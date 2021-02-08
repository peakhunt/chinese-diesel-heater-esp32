import Vue from 'vue'
import Vuex from 'vuex'

import webAPI from './web.js'

/// #if RUN_TARGET == 'electron'
import cliAPI from './cli'
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
    pollTmr: null,
    portName: '',
    availablePorts: [],
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
    },
    SET_PORT_NAME(state, n) {
      state.portName = n
    },
    SET_PORT_LIST(state, pl) {
      state.availablePorts = pl
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
    tempUnit(state) {
      return state.tempUnit
    },
    pollTmr(state) {
      return state.pollTmr
    },
    commPortList(state) {
      return state.availablePorts;
    },
    portName(state) {
      return state.portName
    },
    isCommPortOpen(state) {
      if (state.portName !== '') {
        return true
      }
      return false
    },
  },
  actions: {
    commitHeaterStatus(context, data) {
      context.commit('CHANGE_FAN_RUNNING', data.fanRunning)
      context.commit('CHANGE_FAN_POWER', data.fanPower)
      context.commit('CHANGE_PUMP_RUNNING', data.pumpRunning)
      context.commit('CHANGE_PUMP_FREQ', data.pumpFreq)
      context.commit('PUSH_OUTLET_TEMPERATURE', data.outletTemp)
      context.commit('PUSH_ROOM_TEMPERATURE', 0)   // FIXME
      context.commit('CHANGE_GLOW_PLUG_ON', data.glowPlugOn)
      context.commit('CHANGE_FLAME_DETECTED', data.flameDetected)
      context.commit('CHANGE_STATE', data.state)
    },
    pollStatus(context) {
/// #if RUN_TARGET == 'web'
      context.dispatch('pollStatusWeb')
/// #elif RUN_TARGET == 'electron'
      context.dispatch('pollStatusCLI')
/// #endif 
    },
    fanStart(context, { callback }) {
/// #if RUN_TARGET == 'web'
      context.dispatch('fanStartWeb', { callback })
/// #elif RUN_TARGET == 'electron'
      context.dispatch('fanStartCLI', { callback })
/// #endif 
    },
    fanStop(context, { callback }) {
/// #if RUN_TARGET == 'web'
      context.dispatch('fanStopWeb', { callback })
/// #elif RUN_TARGET == 'electron'
      context.dispatch('fanStopCLI', { callback })
/// #endif 
    },
    fanPower(context, { power, callback }) {
/// #if RUN_TARGET == 'web'
      context.dispatch('fanPowerWeb', { power, callback })
/// #elif RUN_TARGET == 'electron'
      context.dispatch('fanPowerCLI', { power, callback })
/// #endif 
    },
    pumpStart(context, { callback }) {
/// #if RUN_TARGET == 'web'
      context.dispatch('pumpStartWeb', { callback })
/// #elif RUN_TARGET == 'electron'
      context.dispatch('pumpStartCLI', { callback })
/// #endif 
    },
    pumpStop(context, { callback }) {
/// #if RUN_TARGET == 'web'
      context.dispatch('pumpStopWeb', { callback })
/// #elif RUN_TARGET == 'electron'
      context.dispatch('pumpStopCLI', { callback })
/// #endif 
    },
    pumpFreq(context, { freq, callback }) {
/// #if RUN_TARGET == 'web'
      context.dispatch('pumpFreqWeb', { freq, callback })
/// #elif RUN_TARGET == 'electron'
      context.dispatch('pumpFreqCLI', { freq, callback })
/// #endif 
    },
    glowOn(context, { callback }) {
/// #if RUN_TARGET == 'web'
      context.dispatch('glowOnWeb', { callback })
/// #elif RUN_TARGET == 'electron'
      context.dispatch('glowOnCLI', { callback })
/// #endif 
    },
    glowOff(context, { callback }) {
/// #if RUN_TARGET == 'web'
      context.dispatch('glowOffWeb', { callback })
/// #elif RUN_TARGET == 'electron'
      context.dispatch('glowOffCLI', { callback })
/// #endif 
    },
    heaterStart(context, { callback }) {
/// #if RUN_TARGET == 'web'
      context.dispatch('heaterStartWeb', { callback })
/// #elif RUN_TARGET == 'electron'
      context.dispatch('heaterStartCLI', { callback })
/// #endif 
    },
    heaterStop(context, { callback }) {
/// #if RUN_TARGET == 'web'
      context.dispatch('heaterStopWeb', { callback })
/// #elif RUN_TARGET == 'electron'
      context.dispatch('heaterStopCLI', { callback })
/// #endif 
    },
    heaterSettingsGet(context, { callback }) {
/// #if RUN_TARGET == 'web'
      context.dispatch('heaterSettingsGetWeb', { callback })
/// #elif RUN_TARGET == 'electron'
      context.dispatch('heaterSettingsGetCLI', { callback })
/// #endif 
    },
    heaterSettingChange(context, { ndx, value, callback }) {
/// #if RUN_TARGET == 'web'
      context.dispatch('heaterSettingChangeWeb', { ndx, value, callback })
/// #elif RUN_TARGET == 'electron'
      context.dispatch('heaterSettingChangeCLI', { ndx, value, callback })
/// #endif 
    },
    heaterStepChange(context, { ndx, pwr, freq, callback }) {
/// #if RUN_TARGET == 'web'
      context.dispatch('heaterStepChangeWeb', { ndx, pwr, freq, callback })
/// #elif RUN_TARGET == 'electron'
      context.dispatch('heaterStepChangeCLI', { ndx, pwr, freq, callback })
/// #endif 
    },
    startPolling(context) {
/// #if RUN_TARGET == 'web'
      context.dispatch('startPollingWeb', context)
/// #elif RUN_TARGET == 'electron'
      context.dispatch('startPollingCLI', context)
/// #endif 
    },
    stopPolling(context) {
      if (context.getters.pollTmr !== null) {
        clearTimeout(context.getters.pollTmr)
        context.commit('SET_POLL_TMR', null)
      }
    }
  },
  modules: {
    webAPI,
/// #if RUN_TARGET == 'electron'
    cliAPI,
/// #endif
  },
  strict: true
})
