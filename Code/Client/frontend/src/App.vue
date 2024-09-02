<template>
  <div id="app">
    <AuthForm v-if="!isAuthenticated" @auth-success="onAuthSuccess" />
    <div v-else>
      <div class="container" v-if="devices.length > 0">
        <div
            v-for="(device, index) in devices"
            :key="device.uid"
            class="device-card"
        >
          <h1>{{ device.name }}</h1>
          <div v-if="device.type === 0" class="control-button">
            <button
                :class="buttonStates[device.uid] ? 'button-on' : 'button-off'"
                @click="toggleButtonState(device.uid)"
            >
              {{ buttonStates[device.uid] ? 'ON' : 'OFF' }}
            </button>
          </div>
          <div v-else-if="device.type === 1" class="control-form">
            <form @submit.prevent="submitForm(device.uid, formValues[device.uid])">
              <input
                  type="number"
                  v-model.number="formValues[device.uid]"
                  class="form-input"
                  placeholder="Enter an integer"
                  required
              />
              <button type="submit" class="form-submit">Send</button>
            </form>
          </div>
          <div v-else-if="device.type === 2" class="chart-wrapper">
            <ApexChartComponent
                type="line"
                height="350"
                :ref="'chart' + index"
                :options="chartOptionsList[index]"
                :series="seriesData[index]"
            ></ApexChartComponent>
          </div>
        </div>
      </div>

      <br>
      <br>

        <div class="device-card add-device-card">
          <h1>Add New Device</h1>
          <form @submit.prevent="addDevice" class="control-form">
            <input
                type="text"
                v-model="newDevice.name"
                class="form-input"
                placeholder="Device Name"
                required
            />
            <input
                type="text"
                v-model="newDevice.uid"
                class="form-input"
                placeholder="UID"
                required
            />
            <select v-model.number="newDevice.type" class="form-input" required>
              <option disabled value="">Select Type</option>
              <option value=0>Type 0</option>
              <option value=1>Type 1</option>
              <option value=2>Type 2</option>
            </select>
            <button type="submit" class="form-submit">Add Device</button>
          </form>
        </div>
      </div>
    </div>

</template>

<script>
import axios from 'axios';
import AuthForm from "./components/AuthForm";

export default {
  components: {
    AuthForm,
  },
  data() {
    return {
      username: null,
      password: null,
      isAuthenticated: false,
      seriesData: [],
      devices: [],
      buttonStates: {},
      formValues: {},
      chartOptionsList: [],
      newDevice: {
        name: '',
        uid: '',
        type: null,
      },
    };
  },
  mounted() {
    this.username = localStorage.getItem('username');
    this.password = localStorage.getItem('password');
    axios
        .get(`http://127.0.0.1:8000/devices/?username=${encodeURIComponent(this.username)}&password=${encodeURIComponent(this.password)}`)
        .then((response) => {
          const devices = response.data;
          this.devices = devices.map((device) => ({
            name: device[0],
            uid: device[1],
            type: device[2],
          }));

          this.devices.forEach((device, index) => {
            if (device.type === 2) {
              this.seriesData.push([{ name: `Series ${index + 1}`, data: [] }]);
              this.chartOptionsList.push({ ...this.defaultChartOptions });
              this.fetchData(device.uid, index);
            } else if (device.type === 0) {
              this.buttonStates[device.uid] = true;
              this.toggleButtonState(device.uid);
            } else if (device.type === 1) {
              this.formValues[device.uid] = '';
            }
          });

          setInterval(() => {
            this.devices.forEach((device, index) => {
              if (device.type === 2) {
                this.fetchData(device.uid, index);
              }
            });
          }, 5000);
        })
        .catch((error) => {
          console.error('Error fetching devices:', error);
        });
  },
  methods: {
    onAuthSuccess() {
      this.isAuthenticated = true;
    },
    fetchData(uid, index) {
      axios
          .get(`http://127.0.0.1:8000/sensor/data/${uid}/?username=${encodeURIComponent(this.username)}&password=${encodeURIComponent(this.password)}`)
          .then((response) => {
            const data = response.data;
            if (Array.isArray(data)) {
              this.seriesData[index] = [{ data }];
              const currentMaxValue = Math.max(...data.map((point) => point['y']));
              this.chartOptionsList[index] = {
                ...this.chartOptionsList[index],
                yaxis: {
                  max: Math.ceil(currentMaxValue + 1),
                },
              };
              this.$refs[`chart${index}`][0].updateOptions(
                  this.chartOptionsList[index],
                  false,
                  false
              );
            }
          })
          .catch((error) => {
            console.error(`Error fetching data for uid ${uid}:`, error);
          });
    },
    toggleButtonState(uid) {
      const isOn = this.buttonStates[uid];
      const newState = isOn ? 'off' : 'on';
      axios
          .get(`http://127.0.0.1:8000/boolean/${uid}/${newState}/?username=${encodeURIComponent(this.username)}&password=${encodeURIComponent(this.password)}`)
          .then(() => {
            this.buttonStates[uid] = !isOn;
          })
          .catch((error) => {
            console.error(`Error toggling button state for uid ${uid}:`, error);
          });
    },
    submitForm(uid, value) {
      if (value === '' || value === null || isNaN(value)) {
        alert('Please enter a valid number.');
        return;
      }
      const url = `http://bore.pub:49897/integer/${uid}/?value=${encodeURIComponent(
          value
      )}&username=${encodeURIComponent(this.username)}&password=${encodeURIComponent(this.password)}`;
      axios
          .get(url)
          .then(() => {
            this.formValues[uid] = '';
          })
          .catch((error) => {
            console.error(`Error submitting form for uid ${uid}:`, error);
          });
    },
    addDevice() {
      const { name, uid, type } = this.newDevice;
      if (!name || !uid || type === null) {
        alert('Please fill out all fields.');
        return;
      }
      const url = `http://bore.pub:49897/add/?name=${encodeURIComponent(
          name
      )}&uid=${encodeURIComponent(uid)}&type=${type}&username=${encodeURIComponent(this.username)}&password=${encodeURIComponent(this.password)}`;
      axios
          .get(url)
          .then(() => {
            alert('Device added successfully.');
            this.newDevice.name = '';
            this.newDevice.uid = '';
            this.newDevice.type = null;
          })
          .catch((error) => {
            console.error('Error adding new device:', error);
          });
    },
  },
  computed: {
    defaultChartOptions() {
      return {
        chart: {
          id: 'realtime',
          height: 350,
          type: 'line',
          animations: {
            enabled: true,
            easing: 'linear',
            dynamicAnimation: {
              speed: 1000,
            },
          },
          toolbar: {
            show: false,
          },
          zoom: {
            enabled: false,
          },
        },
        dataLabels: {
          enabled: false,
        },
        stroke: {
          curve: 'smooth',
        },
        title: {
          text: '',
          align: 'left',
        },
        markers: {
          size: 0,
        },
        xaxis: {
          type: 'datetime',
          range: 60000,
        },
        yaxis: {
          max: 0,
        },
        legend: {
          show: false,
        },
      };
    },
  },
};
</script>


<style>
#app {
  font-family: Avenir, Helvetica, Arial, sans-serif;
  -webkit-font-smoothing: antialiased;
  -moz-osx-font-smoothing: grayscale;
  text-align: center;
  color: #2c3e50;
  margin-top: 40px;
  display: grid;
  justify-items: center;
  justify-content: center;
  flex-wrap: wrap;
}

.container {
  display: grid;
  grid-template-columns: repeat(2, 1fr);
  gap: 20px;
  width: 100%;
  max-width: 800px;
  padding: 20px;
  background: #f1f1f1;
  border-radius: 8px;
  box-shadow: 0 4px 8px rgba(0, 0, 0, 0.2);
}

@media (max-width: 600px) {
  .container {
    grid-template-columns: 1fr;
  }
}

.device-card {
  background: #fff;
  border-radius: 6px;
  padding: 16px;
  box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
  transition: transform 0.2s;
}

.device-card:hover {
  transform: scale(1.02);
}


button {
  border: none;
  padding: 10px 20px;
  color: white;
  cursor: pointer;
  border-radius: 4px;
}

button.button-on {
  background-color: #28a745;
}

button.button-off {
  background-color: #dc3545;
}

.control-button {
  margin-top: 10px;
}

.control-form {
  margin-top: 10px;
}

.chart-wrapper {
  margin-top: 10px;
}

.form-input {
  padding: 8px;
  margin: 8px 0;
  border: 1px solid #ccc;
  border-radius: 4px;
  width: calc(100% - 16px);
}

form button[type='submit'] {
  background-color: #007bff;
  margin-top: 5px;
}

form button[type='submit']:hover {
  background-color: #0056b3;
}

.add-device-card {
  margin-top: 20px;
}

.add-device-card h1 {
  margin-bottom: 20px;
}

.add-device-card form {
  display: flex;
  flex-direction: column;
  gap: 16px;
}

.add-device-card button {
  background-color: #28a745;
}

.add-device-card button:hover {
  background-color: #218838;
}
</style>
