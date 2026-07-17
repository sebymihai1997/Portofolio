# Portfolio

This repository contains my personal portfolio, engineering projects, and web development work.

## Live Links

- Portfolio site: https://sebymihai1997.github.io/Portofolio
- Blog: https://analizait.wordpress.com/

## Projects Included

### 1. Portfolio Website

- Root website files in `index.html`
- Additional pages and assets in the `website/` folder
- Includes HTML, CSS and JavaScript for a personal landing page and project showcase

### 2. AIQ System (Air Quality Monitoring)

- `AIQ system/AIQ_server/app.py` - Flask backend for collecting air quality telemetry
- Uses MQTT to receive sensor data from ESP32 devices
- Stores readings in SQLite via SQLAlchemy
- Serves a static frontend in `AIQ system/AIQ_server/static/`
- `AIQ system/ESP32 code/` - ESP32 firmware sources for sensors and MQTT publishing

### 3. E-Liquid Calculator

- `Eliquid_Calculator/Eliquid_Calculator.html` - calculator interface for e-liquid mixing
- Validation scripts in `Eliquid_Calculator/VS_Copilot_Verification/`

### 4. Certificates and Documentation

- `Certificate/` contains project certificates and related materials
- `docs/` contains additional documentation resources

## Repository Structure

- `AIQ system/` - air quality monitoring application and firmware
- `website/` - personal portfolio pages, styles, and scripts
- `Eliquid_Calculator/` - e-liquid calculation tool and verification scripts
- `Certificate/` - certificates and credentials
- `docs/` - supporting documentation
- `index.html` - main portfolio landing page

## AIQ System Setup

The Flask app in `AIQ system/AIQ_server/app.py` is configured with environment variables:

- `MQTT_HOST` (default: `localhost`)
- `MQTT_PORT` (default: `1883`)
- `MQTT_TOPIC` (default: `aqm/+/telemetry`)
- `DB_URL` (default: `sqlite:///aqm.sqlite3`)

Required Python packages:

- `flask`
- `sqlalchemy`
- `python-dotenv`
- `paho-mqtt`

## Notes

This repository reflects my interest in embedded systems, web development, data logging, and personal portfolio design. The AIQ project demonstrates a complete IoT data pipeline from ESP32 sensor firmware to backend storage and API delivery.
