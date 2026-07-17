# AIQ Server

Air Quality Monitoring System backend and dashboard.

This project is the Flask-based server for the Air Quality Monitoring (AIQ) system. It ingests MQTT telemetry from ESP32 devices, stores readings in a SQLite database, and serves a static web dashboard for viewing sensor data.

## Contents

- `app.py` - Flask application with MQTT ingestion, database storage, and REST API endpoints.
- `static/` - Web dashboard frontend including `index.html`, `script.js`, and `style.css`.

## Features

- MQTT client subscribes to sensor telemetry topics
- Stores air quality readings in SQLite using SQLAlchemy
- Provides a REST API for latest and historical data
- Serves a dashboard for live monitoring of IAQ, temperature, humidity, VOCs, CO₂, and dust
- Handles timestamp parsing and raw payload storage

## How it works

1. ESP32 devices publish telemetry to MQTT topics matching `aqm/<location>/telemetry`.
2. The Flask app connects to the MQTT broker and saves incoming data into the `readings` table.
3. The frontend fetches `/api/history` and updates charts in real time.

## API Endpoints

- `GET /` - Serves the dashboard from `static/index.html`
- `GET /api/health` - Health check endpoint
- `GET /api/latest` - Returns the latest reading for each location
- `GET /api/history` - Returns historical readings with optional query parameters:
  - `from` - ISO timestamp lower bound
  - `to` - ISO timestamp upper bound
  - `location` - filter by location

## Configuration

The application uses environment variables for configuration. Defaults are provided if variables are missing.

- `MQTT_HOST` - MQTT broker host (`localhost`)
- `MQTT_PORT` - MQTT broker port (`1883`)
- `MQTT_TOPIC` - MQTT topic filter (`aqm/+/telemetry`)
- `DB_URL` - SQLAlchemy database URL (`sqlite:///aqm.sqlite3`)

## Requirements

Install dependencies before running the server:

```bash
pip install flask sqlalchemy python-dotenv paho-mqtt
```

## Run locally

From the `AIQ system/AIQ_server` directory:

```bash
python app.py
```

Then open the dashboard in your browser at `http://localhost:5000/`.

## Notes

- The server uses a background MQTT thread to receive telemetry while the Flask app handles HTTP requests.
- SQLite is configured with WAL mode to improve concurrency and reduce locking issues.
- The web dashboard refreshes every 5 seconds and supports multiple locations with charted historical data.
