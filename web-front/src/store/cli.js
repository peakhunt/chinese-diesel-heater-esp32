import SerialPort from 'serialport'
import HeaterCLI from '../util/heater_cli'

let _cli = null

function splitNameValueAndTrim(l) {
  let [n, v] = l.split(':')
  let values = []

  n = n.trim()
  if (v !== undefined) {
    v = v.trim()
    
    v.split(',').forEach((p) => {
      values.push(p.trim())
    })
  }
  return { n, values }
}

function parseStatus(lines) {
  let data = {
    fanRunning: false,
    fanPower: 0,
    pumpRunning: 0,
    pumpFreq: 0,
    outletTemp: 0,
    glowPlugOn: 0,
    flameDetected: false,
    state: 0,
  }

  lines.forEach((l) => {
    let { n, values } = splitNameValueAndTrim(l) 
    
    switch (n) {
      case 'state':
        switch (values[0]) {
          case 'off':
            data.state = 0
            break

          case 'glowing for start':
            data.state = 1
            break

          case 'priming':
            data.state = 2
            break

          case 'running':
            data.state = 3
            data.flameDetected = true
            break

          case 'glowing for stop':
            data.state = 4
            break

          case 'cooling':
            data.state = 5
            break
        }
        break

      case 'pump':
        if (values[0] === 'off') {
          data.pumpRunning = false
        } else {
          data.pumpRunning = true
        }
        data.pumpFreq = parseFloat(values[1].split(' ')[1])
        break

      case 'glow':
        if (values[0] === 'off') {
          data.glowPlugOn = false
        } else {
          data.glowPlugOn = true
        }
        break

      case 'fan':
        if (values[0] === 'off') {
          data.fanRunning = false
        } else {
          data.fanRunning = true
        }
        data.fanPower = parseInt(values[2].split(' ')[1])
        break

      case 'step':
        break

      case 'outlet':
        data.outletTemp = parseFloat(values[0])
        break
    }
  })

  return data
}

function openCommPort( context, path, callback) {
  console.log('opening comm port')

  const port = new SerialPort(path, {
    autoOpen: false,
    baudRate: 115200,
    dataBits: 8,
    stopBits: 1,
    parity: 'none',
  })

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
      var portList = []

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
        context.dispatch('stopPolling')

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
      if (_cli === null) {
        console.log('port not open')
        return
      }

      _cli.command('status\r', (lines, err) => {
        if (err ) {
          context.commit('CHANGE_COMM_STATUS', false)
          return
        } else {
          let data = parseStatus(lines)

          context.commit('CHANGE_COMM_STATUS', true)
          context.dispatch('commitHeaterStatus', data)
        }
      })
    },
    fanStartCLI(context, { callback }) {
      if (_cli === null) {
        console.log('port not open')
        return
      }

      _cli.command('fan on\r', (lines, err) => {
        if (err) {
          callback(err)
          return
        }
        callback()
      })
    },
    fanStopCLI(context, { callback }) {
      if (_cli === null) {
        console.log('port not open')
        return
      }

      _cli.command('fan off\r', (lines, err) => {
        if (err) {
          callback(err)
          return
        }
        callback()
      })
    },
    fanPowerCLI(context, { power, callback }) {
      if (_cli === null) {
        console.log('port not open')
        return
      }

      _cli.command(`fan_power ${power}\r`, (lines, err) => {
        if (err) {
          callback(err)
          return
        }
        callback()
      })
    },
    pumpStartCLI(context, { callback }) {
      if (_cli === null) {
        console.log('port not open')
        return
      }

      _cli.command(`oil on\r`, (lines, err) => {
        if (err) {
          callback(err)
          return
        }
        callback()
      })
    },
    pumpStopCLI(context, { callback }) {
      if (_cli === null) {
        console.log('port not open')
        return
      }

      _cli.command(`oil off\r`, (lines, err) => {
        if (err) {
          callback(err)
          return
        }
        callback()
      })
    },
    pumpFreqCLI(context, { freq, callback }) {
      if (_cli === null) {
        console.log('port not open')
        return
      }

      _cli.command(`pump_freq ${freq.toFixed(1)}\r`, (lines, err) => {
        if (err) {
          callback(err)
          return
        }
        callback()
      })
    },
    glowOnCLI(context, { callback }) {
      if (_cli === null) {
        console.log('port not open')
        return
      }

      _cli.command(`glow on\r`, (lines, err) => {
        if (err) {
          callback(err)
          return
        }
        callback()
      })
    },
    glowOffCLI(context, { callback }) {
      if (_cli === null) {
        console.log('port not open')
        return
      }

      _cli.command(`glow off\r`, (lines, err) => {
        if (err) {
          callback(err)
          return
        }
        callback()
      })
    },
    heaterStartCLI(context, { callback }) {
      if (_cli === null) {
        console.log('port not open')
        return
      }

      _cli.command(`start\r`, (lines, err) => {
        if (err) {
          callback(err)
          return
        }
        callback()
      })
    },
    heaterStopCLI(context, { callback }) {
      if (_cli === null) {
        console.log('port not open')
        return
      }

      _cli.command(`stop\r`, (lines, err) => {
        if (err) {
          callback(err)
          return
        }
        callback()
      })
    },
    startPollingCLI( { commit, dispatch } ) {
      let polltmr

      polltmr = setInterval(() => {
        dispatch('pollStatusCLI')
      }, 1000)
      commit('SET_POLL_TMR', polltmr)
    },
  },
  strict: true
}
