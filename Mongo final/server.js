const express = require('express');
const bodyParser = require('body-parser');
const mongoose = require('mongoose');

const app = express();
const port = 3000;

// 
mongoose.connect('mongodb+srv://Adham:Adham@cluster0.svnjpqc.mongodb.net/?retryWrites=true&w=majority', { useNewUrlParser: true, useUnifiedTopology: true })
  .then(() => {
    console.log('Connected to MongoDB');
  })
  .catch((err) => {
    console.error('Failed to connect to MongoDB', err);
  });

// 
const timeSettingsSchema = new mongoose.Schema({
  heating: { type: Number, required: true },
  lightMeasurement: { type: Number, required: true },
  pwmComponent: { type: Number, required: true },
  delayBetweenPhases: { type: Number, required: true }
});

// 
const TimeSettings = mongoose.model('TimeSettings', timeSettingsSchema);

app.use(bodyParser.urlencoded({ extended: true }));
app.use(bodyParser.json());

// 
app.get('/settings', (req, res) => {
  TimeSettings.findOne().then((settings) => {
    res.json(settings);
  }).catch((err) => {
    console.error('Failed to retrieve time settings', err);
    res.status(500).json({ error: 'Failed to retrieve time settings' });
  });
});

//
app.put('/settings', (req, res) => {
  const newSettings = req.body;

  TimeSettings.findOneAndUpdate({}, newSettings, { new: true, upsert: true }).then(() => {
    res.json({ message: 'Time settings updated successfully' });
  }).catch((err) => {
    console.error('Failed to update time settings', err);
    res.status(500).json({ error: 'Failed to update time settings' });
  });
});

// 
app.get('/arduino/settings', (req, res) => {
  TimeSettings.findOne().then((settings) => {
    const { heating, lightMeasurement, pwmComponent, delayBetweenPhases } = settings;
    const arduinoSettings = `heating=${heating}&light_measurement=${lightMeasurement}&pwm_component=${pwmComponent}&delay=${delayBetweenPhases}`;
    res.send(arduinoSettings);
  }).catch((err) => {
    console.error('Failed to retrieve time settings', err);
    res.status(500).json({ error: 'Failed to retrieve time settings' });
  });
});

// 
app.listen(port, () => {
  console.log(`Server running on port ${port}`);
});
