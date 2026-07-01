/*
    Master Thesis - Air Quality Monitoring System
    Student: Mihai Sebastian-Andrei
    script.js
    
    
    Frontend JavaScript for Air Quality Monitoring Dashboard
    - Modal info handling
    - Chart rendering with Chart.js
    - Data fetching and live updates
*/

//  MODAL INFO LOGIC
const infoData = {
    acc: `
        <p><strong>Calibration Status (Accuracy)</strong></p>
        <p>The system needs self-calibration to have the best results of air quality.</p>
        <ul>
            <li><span class="txt-red">0 - Stabilization:</span> The sensors has just started. Readings are not yet reliable.</li>
            <li><span class="txt-yellow">1 - Low Accuracy:</span> The sensors are gathering history. It needs exposure to both fresh and polluted air.</li>
            <li><span class="txt-yellow">2 - Medium Accuracy:</span> Calibration is in progress. Readings are getting better.</li>
            <li><span class="txt-green">3 - High Accuracy:</span> The sensors are fully calibrated and trusted.</li>
        </ul>
    `,
    iaq: `
        <p><strong>Index for Air Quality (IAQ)</strong> reflects the overall air condition, combining data from the BME680 sensor.</p>
        <ul>
            <li><span class="txt-green">0 - 50:</span> Excellent (Pure Air)</li>
            <li><span class="txt-green">51 - 100:</span> Good</li>
            <li><span class="txt-yellow">101 - 150:</span> Lightly Polluted (Ventilate room)</li>
            <li><span class="txt-orange">151 - 200:</span> Moderately Polluted</li>
            <li><span class="txt-red">201 - 250:</span> Heavily Polluted</li>
            <li><span class="txt-purple">251 - 350:</span> Severe (Health risk)</li>
            <li><span class="txt-purple">> 351:</span> Extreme</li>
        </ul>`,
    temp: `
        <p>Ambient temperature is measured by the <strong>HTU21D</strong> sensor.</p>
        <ul>
            <li><strong>Optimal Comfort:</strong> 20°C - 24°C</li>
            <li><strong>Too Cold:</strong> Below 18°C</li>
            <li><strong>Too Hot:</strong> Above 26°C</li>
        </ul>`,
    hum: `
        <p>Relative Humidity (RH) is measured by the <strong>HTU21D</strong> sensor.</p>
        <ul>
            <li><strong>Optimal:</strong> 30% - 60%</li>
            <li><strong>Dry Air:</strong> Below 30% (Respiratory irritation)</li>
            <li><strong>Humid (Mold risk):</strong> Above 65%</li>
        </ul>`,
    co2: `
        <p><strong>Equivalent eCO₂ (estimated CO₂)</strong> is calculated by the <strong>SGP30</strong> sensor.</p>
        <ul>
            <li><strong>Excellent:</strong> 400 - 800 ppm</li>
            <li><strong>Acceptable:</strong> 800 - 1000 ppm</li>
            <li><strong>Ventilation Needed:</strong> > 1000 ppm (Drowsiness)</li>
            <li><strong>Dangerous:</strong> > 2000 ppm</li>
        </ul>`,
    voc: `
        <p><strong>Total Volatile Organic Compounds (TVOC)</strong> is measured by the <strong>SGP30</strong> sensor.</p>
        <p>These are gases emitted by cleaning products, paints, cooking, or human.</p>
        <ul>
            <li>Measured in ppb (parts per billion).</li>
            <li>Values under 200 ppb are typical for indoors spaces.</li>
            <li>Spikes can occur during cooking or cleaning.</li>
        </ul>`,
    dust: `
        <p><strong>Fine Particles (Dust)</strong> is measured by the <strong>Sharp GP2Y1014</strong> sensor.</p>
        <p>Microscopic dust that can penetrate the lungs.</p>
        <ul>
            <li><strong>Excellent:</strong> 0 - 10 mg/m³</li>
            <li><strong>Good:</strong> 10 - 25 mg/m³</li>
            <li><strong>Moderate:</strong> 25 - 50 mg/m³</li>
            <li><strong>Poor:</strong> > 50 mg/m³</li>
        </ul>`
};



// Open modal with specific info
function openModal(key) {
    let title = key.toUpperCase();
    if(key === 'iaq') title = "IAQ Index Guide"; // Index for Air Quality
    if(key === 'dust') title = "Dust Info (Sharp GP2Y1014)"; // Fine Particles
    if(key === 'co2') title = "CO2 Info (SGP30)"; // Equivalent CO2
    if(key === 'voc') title = "VOC Info (SGP30)"; // Total Volatile Organic Compounds
    if(key === 'temp') title = "Temperature Info (HTU21D)"; // Temperature
    if(key === 'acc') title = "Sensor Accuracy Info"; // Calibration Status
    
    document.getElementById('mTitle').textContent = title;
    document.getElementById('mBody').innerHTML = infoData[key] || "<p>No info.</p>";
    document.getElementById('infoModal').style.display = 'flex';
}
// Close modal when clicking outside content or on close button
function closeModal(e) {
    if (!e || e.target.classList.contains('modal-overlay') || e.target.classList.contains('modal-close')) {
        document.getElementById('infoModal').style.display = 'none';
    }
}

// Dynamic time window variable
function updateTimeWindow() {
    const selector = document.getElementById('timeSelect'); // Get the select element
    currentWindowMin = parseInt(selector.value); // Get selected value
    
    // Update display
    // Update time window based on selection
    loop(); 
}

//  STANDARD DASHBOARD LOGIC 
const REFRESH_MS = 5000; // Refresh interval in milliseconds      
let currentWindowMin = 60; // Default time window in minutes
// Set initial refresh interval display        
document.getElementById('refreshSec').textContent = (REFRESH_MS/1000)+'s';

const palette = (i)=>['#1f77b4','#ff7f0e','#2ca02c','#d62728','#9467bd','#8c564b'][i%6]; // Color palette for charts
// Create a Chart.js line chart
const mkChart = (canvasId) => {
  const ctx = document.getElementById(canvasId);
  return new Chart(ctx, {
    type: 'line',
    data: { datasets: [] },
    options: {
      animation: false,
      responsive: true,
      maintainAspectRatio: false,
      elements: { point: { radius: 0, hitRadius: 10 } },
      scales: {
        x: { type: 'time', time: { unit: 'minute' }, grid: { display: false } }, // X-axis as time
        y: { beginAtZero: false, grid: { color: '#f0f0f0' } } // Y-axis
      },
      plugins: { legend: { display: false }, tooltip: { mode: 'index', intersect: false } }
    }
  });
};
// Create charts for each parameter
const charts = {
  iaq:  mkChart('iaq'),
  temp: mkChart('temp'),
  rh:   mkChart('rh'),
  co2:  mkChart('co2'),
  voc:  mkChart('voc'),
  dust: mkChart('dust'),
};

let paused = false;
// Pause/Resume button logic
document.getElementById('pauseBtn').onclick = ()=>{
  paused = !paused;
  document.getElementById('pauseBtn').textContent = paused ? 'Resume' : 'Pause';
};
// Update time window based on selection
function groupByLocation(rows) {
  const map = {};
  // Group rows by location
  rows.forEach(r => {
    (map[r.location] = map[r.location] || []).push(r);
  });
  return map;
}

// Update time window based on selection
async function fetchData() {
  const to = new Date();
  const from = new Date(to.getTime() - currentWindowMin *60*1000); // Convert minutes to milliseconds
  
  const url = `/api/history?from=${from.toISOString()}&to=${to.toISOString()}`;// Adjusted URL with dynamic window
  
  const res = await fetch(url);
  return res.json();
}



// Get latest non-null value for a given key
function getLatestValue(rows, key) {
    // Check for empty data
    if (!rows || rows.length === 0) return "--";
    // Iterate backwards to find the latest valid value
    for (let i = rows.length - 1; i >= 0; i--) {
        const val = rows[i][key];
        if (val !== null && val !== undefined) return Number(val); // Return as number
    }
    return "--";
}

// Update live value displays
function updateLiveValues(rows) {
    // Temp & Hum
    const t = getLatestValue(rows, 'temp_c');
    document.getElementById('val_temp').textContent = t !== "--" ? t.toFixed(1) : "--";
    
    const h = getLatestValue(rows, 'rh_pct');
    document.getElementById('val_rh').textContent = h !== "--" ? h.toFixed(1) : "--";
    
    // CO2 & VOC & PM2.5
    const co2 = getLatestValue(rows, 'co2_ppm');
    document.getElementById('val_co2').textContent  = co2 !== "--" ? Math.round(co2) : "--";
    
    const voc = getLatestValue(rows, 'voc_ppb');
    document.getElementById('val_voc').textContent  = voc !== "--" ? Math.round(voc) : "--";
    
    const pm = getLatestValue(rows, 'dust_ugm3');
    document.getElementById('val_dust').textContent = pm !== "--" ? pm.toFixed(1) : "--";

    // IAQ Logic
    const iaq = getLatestValue(rows, 'iaq_score');
    const iaqEl = document.getElementById('val_iaq');
    
    // Set IAQ value and color
    if (iaq !== "--") {
        iaqEl.textContent = Math.round(iaq);
        if(iaq <= 100) iaqEl.style.color = "#2ca02c"; 
        else if(iaq >= 151 && iaq <= 250) iaqEl.style.color = "#ff7f0e"; 
        else iaqEl.style.color = "#d62728"; 
    } else {
        iaqEl.textContent = "--";
        iaqEl.style.color = "#333";
    }

    //  UPDATE GLOBAL ACCURACY HEADER 
    const acc = getLatestValue(rows, 'iaq_acc');
    const accBadge = document.getElementById('global_acc');
    
    // Clear old classes
    accBadge.classList.remove('acc-red', 'acc-yellow', 'acc-green');
    
    let accText = "Accuracy: --";
    // Set accuracy text and color
    if (acc !== "--") {
        accText = "Accuracy: " + Math.round(acc);
        
        if (acc == 0) {
            accBadge.classList.add('acc-red'); 
        } else if (acc < 3) {
            accBadge.classList.add('acc-yellow'); 
        } else {
            accBadge.classList.add('acc-green'); 
        }
    }
    
    // Re-inject HTML to keep the icon
    accBadge.innerHTML = `${accText} <span class="acc-info-icon">i</span>`;
}


// Render charts with fetched data
function render(rows) {
  updateLiveValues(rows); // Update live value displays

  const byLoc = groupByLocation(rows);
  const locNames = Object.keys(byLoc).sort();
  document.getElementById('locs').textContent = locNames.join(', ') || '—'; // Update locations display

  // Build datasets for a given key

  const buildSets = (key, colorIndex) => locNames.map((loc, i) => ({
      label: loc,
      borderColor: palette(i + colorIndex),
      borderWidth: 2,
      data: byLoc[loc]
        .filter(d => d[key] !== null && d[key] !== undefined) // Exclude null/undefined
        .map(d => ({
        x: new Date(d.ts.endsWith("Z") ? d.ts : d.ts + "Z").getTime(), // Ensure UTC
        y: Number(d[key]) // Convert to number
      }))
  }));


// Update datasets for each chart
  charts.iaq.data.datasets  = buildSets('iaq_score', 5);
  charts.temp.data.datasets = buildSets('temp_c', 0);
  charts.rh.data.datasets   = buildSets('rh_pct', 1);
  charts.co2.data.datasets  = buildSets('co2_ppm', 2);
  charts.voc.data.datasets  = buildSets('voc_ppb', 4);
  charts.dust.data.datasets = buildSets('dust_ugm3', 3);

  Object.values(charts).forEach(c => c.update()); // Refresh all charts
}


// Main loop for periodic data fetching and rendering
async function loop() {
  // Only fetch and render if not paused
    if (!paused) {
    try { render(await fetchData()); } catch(e) { console.error(e); }
  }
  setTimeout(loop, REFRESH_MS);
}
loop();