<template>
  <v-dialog
   :value="open"
   transition="dialog-bottom-transition"
   max-width="600"
   @click:outside="$emit('close')"
  >
    <v-card>
      <v-toolbar color="primary" dark>
        Heater
      </v-toolbar>

      <v-card-text>
        <v-simple-table dense>
          <template v-slot:default>
            <thead>
              <tr>
                <th class="text-left">
                Item
                </th>
                <th class="text-left">
                State
                </th>
              </tr>
            </thead>
            <tbody>
              <tr>
                <td>Heater State</td>
                <td>{{ stateStr }}</td>
              </tr>
              <tr>
                <td>Outlet Temperature</td>
                <td>{{outletTemp.toFixed(1)}} °C</td>
              </tr>
              <tr>
                <td>Fan</td>
                <td>{{ fanRunning ? 'Fan running at ' + fanPower + '%' : 'Fan stopped' }}</td>
              </tr>
              <tr>
                <td>Glow Plug</td>
                <td>{{ glowPlugOn ? 'Glow Plug On ' : 'Glow Plug Off' }}</td>
              </tr>
              <tr>
                <td>Fuel Pump</td>
                <td>{{ pumpRunning ? 'Pump running at ' + pumpFreq.toFixed(1) + 'Hz' : 'Pump stopped' }}</td>
              </tr>
            </tbody>
          </template>
        </v-simple-table>
      </v-card-text>

      <v-card-actions class="justify-end">
        <v-btn :disabled="heaterState != 0" text @click="$emit('start')">Start</v-btn>
        <v-btn :disabled="heaterState != 3" text @click="$emit('stop')">Stop</v-btn>
      </v-card-actions>
    </v-card>
  </v-dialog>
</template>

<script>
  import { mapGetters } from 'vuex'

  export default {
    computed: {
      ...mapGetters([
        'heaterState',
        'stateStr',
        'outletTemp',
        'fanRunning',
        'fanPower',
        'glowPlugOn',
        'pumpRunning',
        'pumpFreq',
      ]),
    },
    props: {
      open: {
        type: Boolean,
        default: false,
      },
    },
  }
</script>

