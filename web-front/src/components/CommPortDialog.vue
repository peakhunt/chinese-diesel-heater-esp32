<template>
  <v-dialog
   :value="value"
   @input="$emit('input', $event)"
   transition="dialog-bottom-transition"
   persistent
   max-width="600"
  >
    <v-card>
      <v-toolbar color="primary" dark>
        Comm Port
      </v-toolbar>

      <v-container v-if="!refreshing && !openFailed">
        <v-row align="center">
          <v-col cols="8">
            <v-select
             :items="commPortList"
             v-model="selectedPort"
             label="Comm Port"
            >
            </v-select>
          </v-col>
          <v-col cols="4">
            <v-btn block @click="listCommPorts()">
              Refresh
            </v-btn>
          </v-col>
        </v-row>
      </v-container>

      <v-container v-if="refreshing">
        <v-row class="text-center" aligh="center">
          <v-col cols="12">
            <v-progress-circular
             :size="70"
             :width="7"
             indeterminate
             color="primary"
            ></v-progress-circular>
          </v-col>
        </v-row>
        <v-row class="text-center" aligh="center">
          <v-col cols="12">
            <span> {{ progMessage }} </span>
          </v-col>
        </v-row>
      </v-container>

      <v-container v-if="openFailed">
        <v-row class="text-center" aligh="center">
          <v-col cols="12">
            <span> Opening {{ selectedPort }} Failed!</span>
          </v-col>
        </v-row>
        <v-row class="text-center" aligh="center">
          <v-col cols="12">
            <v-btn text @click="openFailed = false">Ack</v-btn>
          </v-col>
        </v-row>
      </v-container>

      <v-card-actions class="justify-end">
        <v-btn text :disabled="selectedPort === '' || openFailed" @click="onPortOpen()">Open</v-btn>
        <v-btn text :disabled="refreshing" @click="$emit('input', false)">Dismiss</v-btn>
      </v-card-actions>
    </v-card>
  </v-dialog>
</template>

<script>
  import { mapGetters } from 'vuex'

  export default {
    computed: {
      ...mapGetters([
        'commPortList',
        'portName',
      ]),
    },
    props: {
      value: {
        type: Boolean,
        default: false,
      },
    },
    methods: {
      listCommPorts() {
        var self = this

        self.progMessage = 'Refreshing Comm Ports...'
        self.refreshing = true

        self.$store.dispatch('listPorts', (err) => {
          self.refreshing = false
          if (err) {
            console.log(`failed to list comm ports ${err}`)
          }

          if(self.commPortList.includes(self.selectedPort) === false) {
            self.selectedPort = ''
          }
        })
      },
      onPortOpen() {
        var self = this

        self.progMessage = `Opening ${this.selectedPort} ...`
        self.refreshing = true

        self.$store.dispatch('openPort', {
          path: self.selectedPort,
          callback: (err) => {
            self.refreshing = false
            if (err) {
              console.log(`failed to open port ${self.selectedPort}`)
              self.openFailed = true
              self.selectedPort = ''
            } else {
              self.$emit('input', false)
            }
          }
        })
      },
    },
    data: () => ({
      selectedPort: '',
      refreshing: false,
      progMessage: '',
      openFailed: false,
    }),
    watch: {
      value: function(newVal) {
        if (newVal) {
          this.selectedPort = this.portName
          this.openFailed = false
          this.listCommPorts()
        }
      }
    }
  }
</script>
