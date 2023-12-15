<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Kamar Kost - Monitoring</title>
    <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.0.0-beta3/css/all.min.css">
    <link rel="stylesheet" href="styles.css">
</head>

<body>
    <header>
        <h1>Kamar Kost</h1>
    </header>

    <div class="container">
        <div class="section-title">
            <h2>Controls</h2>
        </div>
        <div class="card-container">
            <div class="card">
                <i class="fas fa-power-off"></i>
                <label for="switchAuto">Auto System</label>
                <input type="checkbox" id="switchAuto" class="toggle-switch">
            </div>
            <div class="card">
                <i class="fas fa-lightbulb"></i>
                <label for="switchLamp">Lamp Switch</label>
                <input type="checkbox" id="switchLamp" class="toggle-switch">
            </div>
            <div class="card">
                <i class="fas fa-fan"></i>
                <label for="switchFan">Fan Switch</label>
                <input type="checkbox" id="switchFan" class="toggle-switch">
            </div>
        </div>
        <div class="section-title">
            <h2>Data</h2>
        </div>
        <div class="card-container">
            <div id="temperature" class="card">
                <i class="fas fa-thermometer-half"></i>
                <div class="value">-- °C</div>
                <div class="label">Temperature</div>
            </div>
            <div id="humidity" class="card">
                <i class="fas fa-tint"></i>
                <div class="value">-- %</div>
                <div class="label">Humidity</div>
            </div>
            <div id="light" class="card">
                <i class="fas fa-lightbulb"></i>
                <div class="value">--</div>
                <div class="label">Light Intensity</div>
            </div>
        </div>

        <div class="section-title">
            <h2>Status</h2>
        </div>
        <div class="card-container">
            <div id="lamp" class="card">
                <i class="fas fa-lightbulb"></i>
                <div class="value">--</div>
                <div class="label">Lamp Status</div>
            </div>
            <div id="fan" class="card">
                <i class="fas fa-fan"></i>
                <div class="value">--</div>
                <div class="label">Fan Status</div>
            </div>
        </div>
    </div>


    <footer>
        <p>IOT Kelompok 4</p>
    </footer>

    <script src="app.js"></script>
</body>

</html>