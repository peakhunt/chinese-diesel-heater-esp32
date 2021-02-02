import Vue from 'vue'
import Vuex from 'vuex'

/// #if RUN_TARGET == 'electron'
import CLISerialPort from './cli_serialport'
/// #endif

Vue.use(Vuex)

const stateStrings = [
  "off",
  "glowing for start",
  "priming",
  "running",
  "glowing for stop",
  "cooling",
];

const numMaxTrends = 300

function commitHeaterStatus(context, data) {
  context.commit('CHANGE_FAN_RUNNING', data.fanRunning)
  context.commit('CHANGE_FAN_POWER', data.fanPower)
  context.commit('CHANGE_PUMP_RUNNING', data.pumpRunning)
  context.commit('CHANGE_PUMP_FREQ', data.pumpFreq)
  context.commit('PUSH_OUTLET_TEMPERATURE', data.outletTemp)
  context.commit('PUSH_ROOM_TEMPERATURE', 0)   // FIXME
  context.commit('CHANGE_GLOW_PLUG_ON', data.glowPlugOn)
  context.commit('CHANGE_FLAME_DETECTED', data.flameDetected)
  context.commit('CHANGE_STATE', data.state)
}

function getTempUnitFromLocalStorage() {
  let unit = localStorage.getItem('tempUnit')

  if (unit === null) {
    localStorage.setItem('tempUnit', '°C')
    unit = '°C'
  }
  return unit
}

export default new Vuex.Store({
  state: {
    fanRunning: false,
    fanPower: 0,
    pumpRunning: false,
    pumpFreq: 0.0,
    outletTemp: 0.0,
    glowPlugOn: false,
    flameDetected: false,
    state: 0,
    commStatus: true,
    outletTempTrends: [],
    roomTemp: 0.0,
    roomTempTrends: [],
    tempUnit: getTempUnitFromLocalStorage(),
    polltmr: null,
  },
  mutations: {
    CHANGE_FAN_RUNNING(state, v) {
      state.fanRunning = v
    },
    CHANGE_FAN_POWER(state, v) {
      state.fanPower = v
    },
    CHANGE_PUMP_RUNNING(state, v) {
      state.pumpRunning = v
    },
    CHANGE_PUMP_FREQ(state, v) {
      state.pumpFreq = v
    },
    CHANGE_GLOW_PLUG_ON(state, v) {
      state.glowPlugOn = v
    },
    CHANGE_FLAME_DETECTED(state, v) {
      state.flameDetected = v
    },
    CHANGE_STATE(state, v) {
      state.state = v
    },
    CHANGE_COMM_STATUS(state, v) {
      state.commStatus = v
    },
    PUSH_OUTLET_TEMPERATURE(state, v) {
      //
      // reported temperature from esp32 is always in C
      //
      if (state.tempUnit !== '°C') {
        v = (v * 9/5) + 32
      }

      state.outletTemp = v
      state.outletTempTrends.push(v)

      if (state.outletTempTrends.length > numMaxTrends) {
        state.outletTempTrends.shift()
      }
    },
    PUSH_ROOM_TEMPERATURE(state, v) {
      //
      // reported temperature from esp32 is always in C
      //
      if (state.tempUnit !== '°C') {
        v = (v * 9/5) + 32
      }

      state.roomTemp = v
      state.roomTempTrends.push(v)

      if (state.roomTempTrends.length > numMaxTrends) {
        state.roomTempTrends.shift()
      }
    },
    TOGGLE_TEMP_UNIT(state) {
      if (state.tempUnit === '°C') {
        state.tempUnit = '°F'
      } else {
        state.tempUnit = '°C'
      }

      localStorage.setItem('tempUnit', state.tempUnit)

      state.roomTempTrends = []
      state.outletTempTrends = []
    },
    SET_POLL_TMR(state, tmr) {
      state.polltmr = tmr
    }
  },
  getters: {
    fanRunning(state) {
      return state.fanRunning
    },
    fanPower(state) {
      return state.fanPower
    },
    pumpRunning(state) {
      return state.pumpRunning
    },
    pumpFreq(state) {
      return state.pumpFreq
    },
    outletTemp(state) {
      return state.outletTemp
    },
    roomTemp(state) {
      return state.roomTemp
    },
    glowPlugOn(state) {
      return state.glowPlugOn
    },
    flameDetected(state) {
      return state.flameDetected
    },
    stateStr(state) {
      return stateStrings[state.state]
    },
    heaterState(state) {
      return state.state
    },
    commStatus(state) {
      return state.commStatus
    },
    outletTempTrends(state) {
      return state.outletTempTrends
    },
    roomTempTrends(state) {
      return state.roomTempTrends
    },
    tempUnit(state) {
      return state.tempUnit
    },
    pollTmr(state) {
      return state.pollTmr
    }
  },
  actions: {
    pollStatus(context) {
      Vue.axios.get('/api/v1/heater/status').then((response) => {
        if (response.status !== 200) {
          context.commit('CHANGE_COMM_STATUS', false)
          return
        }

        context.commit('CHANGE_COMM_STATUS', true)
        commitHeaterStatus(context, response.data)
      })
      .catch(function () {
        //
        // test code
        //
        /*
        const o = Math.ceil(Math.random() * 300 - 100)
        const r = Math.ceil(Math.random() * 300 - 100)
        context.commit('PUSH_OUTLET_TEMPERATURE', o)
        context.commit('PUSH_ROOM_TEMPERATURE', r)
        */

        context.commit('CHANGE_COMM_STATUS', false)
      })
    },
    fanStart(context, { callback }) {
      Vue.axios.post('/api/v1/heater/fan/start').then((response) => {
        commitHeaterStatus(context, response.data)
        callback()
      })
      .catch(function (err) {
        callback(err)
      })
    },
    fanStop(context, { callback }) {
      Vue.axios.post('/api/v1/heater/fan/stop').then((response) => {
        commitHeaterStatus(context, response.data)
        callback()
      })
      .catch(function (err) {
        callback(err)
      })
    },
    fanPower(context, { power, callback }) {
      Vue.axios.post('/api/v1/heater/fan/power', { power }).then((response) => {
        commitHeaterStatus(context, response.data)
        callback()
      })
      .catch(function (err) {
        callback(err)
      })
    },
    pumpStart(context, { callback }) {
      Vue.axios.post('/api/v1/heater/pump/start').then((response) => {
        commitHeaterStatus(context, response.data)
        callback()
      })
      .catch(function (err) {
        callback(err)
      })
    },
    pumpStop(context, { callback }) {
      Vue.axios.post('/api/v1/heater/pump/stop').then((response) => {
        commitHeaterStatus(context, response.data)
        callback()
      })
      .catch(function (err) {
        callback(err)
      })
    },
    pumpFreq(context, { freq, callback }) {
      Vue.axios.post('/api/v1/heater/pump/freq', { freq }).then((response) => {
        commitHeaterStatus(context, response.data)
        callback()
      })
      .catch(function (err) {
        callback(err)
      })
    },
    glowOn(context, { callback }) {
      Vue.axios.post('/api/v1/heater/glowplug/on').then((response) => {
        commitHeaterStatus(context, response.data)
        callback()
      })
      .catch(function (err) {
        callback(err)
      })
    },
    glowOff(context, { callback }) {
      Vue.axios.post('/api/v1/heater/glowplug/off').then((response) => {
        commitHeaterStatus(context, response.data)
        callback()
      })
      .catch(function (err) {
        callback(err)
      })
    },
    heaterStart(context, { callback }) {
      Vue.axios.post('/api/v1/heater/start').then((response) => {
        commitHeaterStatus(context, response.data)
        callback()
      })
      .catch(function (err) {
        callback(err)
      })
    },
    heaterStop(context, { callback }) {
      Vue.axios.post('/api/v1/heater/stop').then((response) => {
        commitHeaterStatus(context, response.data)
        callback()
      })
      .catch(function (err) {
        callback(err)
      })
    },
    heaterSettingsGet(context, { callback }) {
      Vue.axios.get('/api/v1/heater/settings')
      .then((response) => {

        if (response.status !== 200) {
          callback(null,'status != 200')
        } else {
          callback(response.data)
        }
      })
      .catch(function () {
        callback(null, 'exception')
      })
    },
    heaterSettingChange(context, { ndx, value, callback }) {
      Vue.axios.post('/api/v1/heater/settings/mod',
        { ndx, value }
      )
      .then((response) => {

        if (response.status !== 200) {
          callback(null, 'status != 200')
        }
        callback(response.data)
      })
      .catch(function() {
        callback(null, 'exception')
      })
    },
    heaterStepChange(context, { ndx, pwr, freq, callback }) {
      Vue.axios.post('/api/v1/heater/settings/step',
        { ndx, pwr, freq }
      )
      .then((response) => {

        if (response.status !== 200) {
          callback(null, 'status != 200')
        }
        callback(response.data)
      })
      .catch(function() {
        callback(null, 'exception')
      })
    },
    startPolling( { commit, dispatch } ) {
      let polltmr

      polltmr = setInterval(() => {
        dispatch('pollStatus')
      }, 1000)
      commit('SET_POLL_TMR', polltmr)
    },
  },
  modules: {
/// #if RUN_TARGET == 'electron'
    CLISerialPort,
/// #endif
  },
  strict: true
})
