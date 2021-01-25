<!--

this is a copy & paste from a brillian source I found in the internet!!!

-->
<template>
<svg
  :width="width"
  :height="height"
  role="presentation"
  viewBox="0 -20 120 240">
  <defs>
    <linearGradient id="fire-gradient-basic" x1="0%" y1="0%" x2="0%" y2="100%">
      <stop stop-color="#ffb200" offset="0.2" />
      <stop stop-color="#dc0000" offset="1" />
    </linearGradient>
    <linearGradient id="fire-gradient-red" x1="0%" y1="0%" x2="0%" y2="100%">
      <stop stop-color="#ffb200" offset="0" />
      <stop stop-color="#dc0000" offset="0.9" />
    </linearGradient>
    <linearGradient id="fire-gradient-yellow" x1="0%" y1="0%" x2="0%" y2="100%">
      <stop stop-color="#ffb200" offset="0.3" />
      <stop stop-color="#dc0000" offset="1" />
    </linearGradient>

    <linearGradient id="dark-black" x1="0%" y1="0%" x2="0%" y2="100%">
      <stop stop-color="#000000" offset="0" />
      <stop stop-color="#000000" offset="1" />
    </linearGradient>
  </defs>

  <g stroke-width="0.5" stroke-opacity="0.5" ref="fire">
    <path class="spark" :fill="sparkFill" stroke="#ff0" d="M13 90c-8,-10 -6,-14 -5,-21 3,-3 0,-7 -2,-19 8,11 18,12 7,40l0 0z"></path>
    <path class="spark" :fill="sparkFill" stroke="#ff0" d="M36 45c8,-10 6,-14 5,-21 -3,-3 0,-7 2,-19 -8,11 -18,12 -7,40l0 0z"></path>
    <path class="spark" :fill="sparkFill" stroke="#ff0" d="M63 48c8,-10 6,-14 5,-21 -3,-3 0,-7 2,-19 -8,11 -18,12 -7,40l0 0z"></path>
    <path class="spark" :fill="sparkFill" stroke="#ff0" d="M83 80c-8,-10 -6,-14 -5,-21 3,-3 0,-7 -2,-19 8,11 18,12 7,40l0 0z"></path>
    <path class="spark" :fill="sparkFill" stroke="#ff0" d="M94 126c8,-10 6,-14 5,-21 -3,-3 0,-7 2,-19 -8,11 -18,12 -7,40l0 0z"></path>
    <path class="flame" :fill="gradientBasic" stroke="#ff0" d="M46 50c1,13 -15,26 -13,44 0,11 -4,-17 -2,-26 -1,-8 -3,-16 -9,-21 8,42 -28,33 -5,88 -4,-2 -8,-4 -12,-7 4,13 9,21 15,28 9,9 21,13 39,16 13,4 28,2 38,-6 10,-7 15,-19 12,-34 -3,6 -7,10 -13,12 3,-17 -14,-25 -7,-42 10,-20 3,-45 0,-32 -4,12 -11,21 -20,29 22,-70 -10,-53 -11,-88 -9,12 -14,23 -12,39z"></path>
    <path class="flame" :fill="gradientYellow" stroke="#ff0" d="M51 168c-55,-58 30,-62 -11,-121 11,39 -13,64 -21,76 -5,-15 -1,-21 5,-39 -38,42 -9,63 27,84l0 0z"></path>
    <path class="flame" :fill="gradientYellow" stroke-width="0.5" stroke="#ff0" d="M55 168c-1,-14 -3,-25 15,-38 31,-22 16,-34 -3,-57 5,22 -4,40 -12,52 -5,8 -23,23 0,43z"></path>
    <path class="flame" :fill="gradientRed" stroke="#ff0" d="M63 172c9,-5 11,-20 12,-37 2,-17 7,-33 34,-36 -11,9 -20,20 -16,41 -1,31 -24,29 -30,32l0 0z"></path>
    <rect x="0" y="0" width="120" height="240" style="fill-opacity: 0" @click="$emit('click')"/>
  </g>
</svg>
</template>

<script>
  import gsap from 'gsap'

  export default {
    name: 'FanIcon',
    props: {
      width: {
        type: [Number, String],
        default: 18
      },
      height: {
        type: [Number, String],
        default: 18
      },
      running: {
        type: Boolean,
        default: false,
      },
    },
    mounted: function () {
      const fire = this.$refs.fire
      const sparks = Array.from(fire.querySelectorAll('.spark'));

      sparks.forEach((s) => {
        let tl = new gsap.timeline({
          repeat: -1,
          paused: true,
          onRepeat: () => {
            tl.duration(Math.random() + 0.2)
          }
        })
        .fromTo(s, {
          duration: 0.2,
          scale: 2,
          y: 40,
          transformOrigin: 'center bottom'
        }, {
          scale: 0,
          y: -60
        });
        this.fireTls.push(tl);
      });

      const flames = Array.from(fire.querySelectorAll('.flame'));
      flames.forEach((f, idx) => {
        let tl = new gsap.timeline({
          repeat: -1,
          paused: true,
          onRepeat: () => {
            tl.duration(Math.random() * 0.4 + 0.2)
          }
        })
        .to(f, 0.2, {
          scaleY: 1.1 + (idx ? 0.2 : 0),
          scaleX: idx ? 1 : 0.8,
          transformOrigin: 'center bottom',
          repeat: 1,
          yoyo: true
        });
        this.fireTls.push(tl);
      });

      if(this.running) {
        this.playFire()
      }
    },
    methods: {
      playFire() {
        this.fireTls.forEach((tl) => {
          tl.play();
        });
        this.animating = true
      },
      pauseFire() {
        this.fireTls.forEach((tl) => {
          tl.pause(0);
        });
        this.animating = false
      },
    },
    data: () => ({
      animating: false,
			fireTls: [],
    }),
    computed: {
      gradientBasic() {
        if (this.running) {
          return 'url(#fire-gradient-basic)'
        }
        return 'url(#dark-black)'
      },
      gradientYellow() {
        if (this.running) {
          return 'url(#fire-gradient-yellow)'
        }
        return 'url(#dark-black)'
      },
      gradientRed() {
        if (this.running) {
          return 'url(#fire-gradient-red)'
        }
        return 'url(#dark-black)'
      },
      sparkFill() {
        if (this.running) {
          return '#ffd700'
        }
        return '#000000'
      },
    },
    watch: {
      running: function(newVal) {
        if (newVal) {
          this.playFire()
        } else {
          this.pauseFire()
        }
      }
    }
  }
</script>
