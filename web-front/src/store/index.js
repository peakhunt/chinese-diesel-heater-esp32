import Vue from 'vue'
import Vuex from 'vuex'

Vue.use(Vuex)

export default new Vuex.Store({
  state: {
    fanRunning: false,
    fanPower: 0,
    pumpRunning: false,
    pumpFreq: 0.0,
    outletTemp: 0.0,
    glowPlugOn: false,
    flameDetected: false,
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
    CHANGE_OUTLET_TEMP(state, v) {
      state.outletTemp = v
    },
    CHANGE_GLOW_PLUG_ON(state, v) {
      state.glowPlugOn = v
    },
    CHANGE_FLAME_DETECTED(state, v) {
      state.flameDetected = v
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
    glowPlugOn(state) {
      return state.glowPlugOn
    },
    flameDetected(state) {
      return state.flameDetected
    },
  },
  actions: {
  },
  modules: {
  },
  strict: true
})
