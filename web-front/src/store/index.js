import Vue from 'vue'
import Vuex from 'vuex'

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
      state.outletTemp = v
      state.outletTempTrends.push(v)

      if (state.outletTempTrends.length > numMaxTrends) {
        state.outletTempTrends.shift()
      }
    },
    PUSH_ROOM_TEMPERATURE(state, v) {
      state.roomTemp = v
      state.roomTempTrends.push(v)

      if (state.roomTempTrends.length > numMaxTrends) {
        state.roomTempTrends.shift()
      }
    },
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
  },
  modules: {
  },
  strict: true
})
