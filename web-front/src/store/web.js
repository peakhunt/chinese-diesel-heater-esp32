import Vue from 'vue'

export default {
  actions: {
    pollStatusWeb(context) {
      Vue.axios.get('/api/v1/heater/status').then((response) => {
        if (response.status !== 200) {
          context.commit('CHANGE_COMM_STATUS', false)
          return
        }

        context.commit('CHANGE_COMM_STATUS', true)
        context.commit('commitHeaterStatus', response.data)
      })
      .catch(function () {
        context.commit('CHANGE_COMM_STATUS', false)
      })
    },
    fanStartWeb(context, { callback }) {
      Vue.axios.post('/api/v1/heater/fan/start').then((response) => {
        context.commit('commitHeaterStatus', response.data)
        callback()
      })
      .catch(function (err) {
        callback(err)
      })
    },
    fanStopWeb(context, { callback }) {
      Vue.axios.post('/api/v1/heater/fan/stop').then((response) => {
        context.commit('commitHeaterStatus', response.data)
        callback()
      })
      .catch(function (err) {
        callback(err)
      })
    },
    fanPowerWeb(context, { power, callback }) {
      Vue.axios.post('/api/v1/heater/fan/power', { power }).then((response) => {
        context.commit('commitHeaterStatus', response.data)
        callback()
      })
      .catch(function (err) {
        callback(err)
      })
    },
    pumpStartWeb(context, { callback }) {
      Vue.axios.post('/api/v1/heater/pump/start').then((response) => {
        context.commit('commitHeaterStatus', response.data)
        callback()
      })
      .catch(function (err) {
        callback(err)
      })
    },
    pumpStopWeb(context, { callback }) {
      Vue.axios.post('/api/v1/heater/pump/stop').then((response) => {
        context.commit('commitHeaterStatus', response.data)
        callback()
      })
      .catch(function (err) {
        callback(err)
      })
    },
    pumpFreqWeb(context, { freq, callback }) {
      Vue.axios.post('/api/v1/heater/pump/freq', { freq }).then((response) => {
        context.commit('commitHeaterStatus', response.data)
        callback()
      })
      .catch(function (err) {
        callback(err)
      })
    },
    glowOnWeb(context, { callback }) {
      Vue.axios.post('/api/v1/heater/glowplug/on').then((response) => {
        context.commit('commitHeaterStatus', response.data)
        callback()
      })
      .catch(function (err) {
        callback(err)
      })
    },
    glowOffWeb(context, { callback }) {
      Vue.axios.post('/api/v1/heater/glowplug/off').then((response) => {
        context.commit('commitHeaterStatus', response.data)
        callback()
      })
      .catch(function (err) {
        callback(err)
      })
    },
    heaterStartWeb(context, { callback }) {
      Vue.axios.post('/api/v1/heater/start').then((response) => {
        context.commit('commitHeaterStatus', response.data)
        callback()
      })
      .catch(function (err) {
        callback(err)
      })
    },
    heaterStopWeb(context, { callback }) {
      Vue.axios.post('/api/v1/heater/stop').then((response) => {
        context.commit('commitHeaterStatus', response.data)
        callback()
      })
      .catch(function (err) {
        callback(err)
      })
    },
    heaterSettingsGetWeb(context, { callback }) {
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
    heaterSettingChangeWeb(context, { ndx, value, callback }) {
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
    heaterStepChangeWeb(context, { ndx, pwr, freq, callback }) {
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
    startPollingWeb( { commit, dispatch } ) {
      let polltmr

      polltmr = setInterval(() => {
        dispatch('pollStatusWeb')
      }, 1000)
      commit('SET_POLL_TMR', polltmr)
    },
  },
}
