import SerialPort from 'serialport'
/*
const Readline = require('@serialport/parser-readline')
const port = new SerialPort('COM8')
const parser = port.pipe(new Readline({ delimiter: '\r\n'}))
*/

export default {
  state: () => ({
    portName: '',
    port: null,
    parser: null,
    availablePorts: [],
  }),

  mutations: {
    SET_PORT_NAME(state, n) {
      state.portName = n
    },
    SET_PORT(state, p) {
      state.port = p
    },
    SET_PARSER(state, pa) {
      state.parser = pa
    },
    SET_PORT_LIST(state, pl) {
      state.availablePorts = pl
    },
  },
  getters: {
  },
  actions: {
    listPorts(context, callback) {
      var portList = [];

      SerialPort.list()
      .then((ports) => {
        ports.forEach((port) => {
          portList.push(port.path)
        })

        context.commit('SET_PORT_LIST', portList)
        callback(null, portList)
      })
      .catch(function() {
        callback('error')
      })
    },
  },
  strict: true
}
