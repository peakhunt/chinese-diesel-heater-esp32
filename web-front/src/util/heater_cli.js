const transaction_timeout = 2000
const cmd_prompt = 'Heater> '

function _executeNext(heater_cli) {
  heater_cli._transaction = null

  if (heater_cli._queue.length === 0) {
    return
  }

  let t = heater_cli._queue.shift()

  _executeTransaction(heater_cli, t)
}

function _executeTransaction(heater_cli, t) {
  heater_cli._transaction   = t
  heater_cli._received      = ''

  heater_cli._port.write(t.cmd)
  heater_cli._timeout = setTimeout(() => {
    t.callback(null, 'transaction timeout')
    _executeNext(heater_cli)
  }, transaction_timeout)
}

var HeaterCLI = function(port) {
  this._port        = port
  this._timeout     = null
  this._transaction = null
  this._queue       = []
  this._received    = ''
}

HeaterCLI.prototype.open = function(callback) {
  let heater_cli = this

  heater_cli._port.open(function(err) {
    if (err) {
      callback(err)
    } else {
      heater_cli._port.on('data', function(data) {
        let t = heater_cli._transaction

        if (t == null) {
          //
          // silently ignore if no transaction is going on
          //
          return
        }
        heater_cli._received += data

        let lines = heater_cli._received.split('\r\n')

        if (lines.length == 0) {
          return
        }

        //
        // if last line matches the prompt
        //
        if (lines[lines.length - 1] === cmd_prompt) {
          //
          // got it
          //
          clearTimeout(heater_cli._timeout)
          t.callback(lines)
          _executeNext(heater_cli)
        }
      })
      callback()
    }
  })
}

HeaterCLI.prototype.close = function(callback) {
  if (this._port) {
    this._port.removeAllListeners("data")
    this._port.close(callback)
  }
}

Object.defineProperty(HeaterCLI.prototype, "isOpen", {
  enumerable: true,
  get: function() {
    if (this._port) {
      return this._port.isOpen
    }
    return false
  }
})

HeaterCLI.prototype.command = function(cmd, callback) {
  if (this.isOpen === false) {
    callback(null, 'port not open')
    return
  }

  let t = {
    cmd,
    callback,
  }

  if (this._transaction !== null) {
    // transaction already in progress
    // queue new transaction
    this._queue.push(t)
    return
  }
  _executeTransaction(this, t)
}

module.exports = HeaterCLI
