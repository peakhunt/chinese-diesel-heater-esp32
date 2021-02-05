import SerialPort from 'serialport'
import HeaterCLI from '../util/heater_cli'

let _cli = null

function openCommPort( context, path, callback) {
  console.log('opening comm port')

  const port = new SerialPort(path, {
    autoOpen: false,
    baudRate: 115200,
    dataBits: 8,
    stopBits: 1,
    parity: 'none',
  });

  const c = new HeaterCLI(port)
  c.open((err) => {
    console.log('opening comm port callback')
    if (!err) {
      console.log('port open complete')
      context.commit('SET_PORT_NAME', path)
      _cli = c
    } else {
      console.log(`port open failed ${err}`)
    }
    callback(err)
  })
}

export default {
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
        context.commit('SET_PORT_LIST', [])
        callback('error')
      })
    },
    openPort(context, { path, callback }) {
      if (_cli !== null) {
        console.log('closing comm port')
        _cli.close((err) => {
          if (err) {
            console.log(`error in closing comm port ${err}`)
          }
          _cli = null
          context.commit('SET_PORT_NAME', '')
          console.log('closing comm port complete')
          openCommPort(context, path, callback)
        })
      } else {
        openCommPort(context, path, callback)
      }
    },
    pollStatusCLI(context) {
      if (_cli !== null) {
        console.log('port not open')
        return
      }

      _cli.command('status\r\n', (lines, err) => {
        let data = {
        }

        if (err ) {
          console.log(`status command failed ${err}`)
          context.commit('CHANGE_COMM_STATUS', false)
          return
        } else {
          //
          // FIXME parse
          //
          context.commit('CHANGE_COMM_STATUS', true)
          context.commit('commitHeaterStatus', data)
        }
      })
    },
  },
  strict: true
}
