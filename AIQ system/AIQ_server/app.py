##      Master Thesis - Air Quality Monitoring System
##      Student: Mihai Sebastian-Andrei

##    app.py
##    Flask application for Air Quality Monitoring System
##    - Receives MQTT messages from ESP32 devices
##    - Stores data in a SQL database using SQLAlchemy
##    - Provides REST API endpoints for data retrieval
##    - Serves a static frontend for data visualization


import json
import os
import threading
from datetime import datetime, timezone

from dotenv import load_dotenv
from flask import Flask, jsonify, request, send_from_directory
from sqlalchemy import Column, Integer, Float, String, DateTime, create_engine, text , event
from sqlalchemy.orm import DeclarativeBase, Session, Mapped, mapped_column
import paho.mqtt.client as mqtt

load_dotenv()

#  Configuration
MQTT_HOST = os.getenv("MQTT_HOST", "localhost")
MQTT_PORT = int(os.getenv("MQTT_PORT", "1883"))
MQTT_TOPIC = os.getenv("MQTT_TOPIC", "aqm/+/telemetry")
DB_URL     = os.getenv("DB_URL", "sqlite:///aqm.sqlite3")

# Database Setup
class Base(DeclarativeBase):
    pass

# Database Model
class Reading(Base):
    __tablename__ = "readings"
    id: Mapped[int] = mapped_column(Integer, primary_key=True)
    ts: Mapped[datetime] = mapped_column(DateTime(timezone=True), index=True)
    location: Mapped[str] = mapped_column(String(64), index=True)
    temp_c: Mapped[float] = mapped_column(Float, nullable=True)
    rh_pct: Mapped[float]  = mapped_column(Float, nullable=True)
    co2_ppm: Mapped[float] = mapped_column(Float, nullable=True)
    voc_ppb: Mapped[float] = mapped_column(Float, nullable=True)
    dust_ugm3: Mapped[float] = mapped_column(Float, nullable=True)
    iaq_score: Mapped[float] = mapped_column(Float, nullable=True)
    iaq_acc: Mapped[float] = mapped_column(Float, nullable=True)
    rssi_dbm: Mapped[float] = mapped_column(Float, nullable=True)
    raw: Mapped[str] = mapped_column(String, nullable=True)

engine = create_engine(DB_URL, echo=False, future=True, connect_args={'timeout': 15}) # SQLite specific timeout to avoid "database is locked" errors

@event.listens_for(engine, "connect")
# SQLite PRAGMA settings
def set_sqlite_pragma(dbapi_connection, connection_record):
    cursor = dbapi_connection.cursor()
    cursor.execute("PRAGMA journal_mode=WAL")
    cursor.close()

Base.metadata.create_all(engine) # Create tables if they don't exist

# Flask App 
app = Flask(__name__, static_folder="static", static_url_path="")

# API and Frontend Endpoints
@app.get("/")
def index():
    return send_from_directory("static", "index.html")
# Health Check Endpoint
@app.get("/api/health")
def health():
    """Simple health check endpoint."""
    with Session(engine) as s:
        s.execute(text("SELECT 1"))
    return {"status": "ok"}

#  API Endpoints
@app.get("/api/latest")
def latest():
    """Returns the latest reading for each location."""
    with Session(engine) as s:
        rows = s.execute(text("""
            SELECT r.*
            FROM readings r
            JOIN (
              SELECT location, MAX(ts) AS max_ts
              FROM readings
              GROUP BY location
            ) m ON r.location = m.location AND r.ts = m.max_ts
            ORDER BY r.location
        """)).mappings().all()
    return jsonify([dict(row) for row in rows])

# Historical Data Endpoint
@app.get("/api/history")
def history():
    """
    Returns historical data.
    Query parameters: ?from=ISO&to=ISO&location=livingroom
    """
    qs_from = request.args.get("from")
    qs_to   = request.args.get("to")
    loc     = request.args.get("location")
    # Build query
    with Session(engine) as s:
        q = s.query(Reading)
        if loc:
            q = q.filter(Reading.location == loc)
        if qs_from:
            q = q.filter(Reading.ts >= datetime.fromisoformat(qs_from))
        if qs_to:
            q = q.filter(Reading.ts <= datetime.fromisoformat(qs_to))
        q = q.order_by(Reading.ts.asc())
        data = [
            {
                "ts": r.ts.isoformat(),
                "location": r.location,
                "temp_c": r.temp_c,
                "rh_pct": r.rh_pct,
                "co2_ppm": r.co2_ppm,
                "voc_ppb": r.voc_ppb,
                "dust_ugm3": r.dust_ugm3,
                "iaq_score": r.iaq_score,
                "iaq_acc": r.iaq_acc,
                "rssi_dbm": r.rssi_dbm,
            } for r in q.all()
        ]
    return jsonify(data)

# MQTT Ingestion
def on_connect(client, userdata, flags, rc, properties=None):
    if rc == 0:
        print(f"[MQTT] Connected. Subscribing to {MQTT_TOPIC}")
        client.subscribe(MQTT_TOPIC, qos=1)
    else:
        print(f"[MQTT] Connect failed rc={rc}")

# MQTT Message Handler
def on_message(client, userdata, msg):
    topic_parts = msg.topic.split("/")
    # Expected format: aqm/<location>/telemetry
    location = topic_parts[1] if len(topic_parts) >= 3 else "unknown"
    try: # Parse JSON payload
        payload = json.loads(msg.payload.decode("utf-8"))
    except Exception: # Invalid JSON
        payload = {}

    # Timestamp handling: prefer payload['ts'], otherwise use current UTC time
    ts_str = payload.get("ts")
    if ts_str:
        try: # parse ISO format
            ts = datetime.fromisoformat(ts_str.replace("Z","+00:00"))
        except Exception: # invalid format
            ts = datetime.now(timezone.utc)
    else: # no timestamp in payload
        ts = datetime.now(timezone.utc)
    # Create Reading row
    row = Reading(
        ts=ts,
        location=location,
        # Mapping ESP32 keys to Database columns
        temp_c    = payload.get("temp"),
        rh_pct    = payload.get("hum"),
        co2_ppm   = payload.get("co2"),
        voc_ppb   = payload.get("tvoc"),
        dust_ugm3 = payload.get("dust"),
        iaq_score = payload.get("iaq"),
        iaq_acc   = payload.get("acc"),
        rssi_dbm  = payload.get("rssi_dbm"),
        raw       = json.dumps(payload) if payload else msg.payload.decode("utf-8", "ignore"), # Store raw payload
    )
    
    print(f"[DATA] Saved: {payload}")
    # Store in database
    with Session(engine) as s:
        s.add(row)
        s.commit()

# MQTT Client Setup
def start_mqtt():
    client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
    client.on_connect = on_connect
    client.on_message = on_message
    client.connect(MQTT_HOST, MQTT_PORT, keepalive=60)
    client.loop_forever()

# Run MQTT client in a separate thread
def run_mqtt_in_background():
    # Run MQTT client in a separate daemon thread
    th = threading.Thread(target=start_mqtt, daemon=True)
    th.start()

# Main entry point
if __name__ == "__main__":
    run_mqtt_in_background()
    app.run(host="0.0.0.0", port=5000, debug=True)