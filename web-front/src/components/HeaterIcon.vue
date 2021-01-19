<template>
<svg
  xmlns="http://www.w3.org/2000/svg"
  :width="width"
  :height="height"
  viewBox="0 0 456.13867 189.4668">

  <g :fill="iconColor" ref="heaterIcon">
    <path
     d="M 443.13868,94.720701 V 122.9668 h -12.47461 a 6.50065,6.50065 0 0 0 -5.63086,3.25 c -9.67078,16.75018 -19.34094,33.49981 -29.01172,50.25 H 228.07227 v 13 h 171.70117 a 6.50065,6.50065 0 0 0 5.63086,-3.25 c 9.67078,-16.75019 19.34094,-33.49982 29.01172,-50.25 h 15.22266 a 6.50065,6.50065 0 0 0 6.5,-6.5 V 94.720701 Z m -443.1386870712501,0 V 129.4668 A 6.50065,6.50065 0 0 0 6.4999929,135.9668 H 21.724603 c 9.67073,16.75018 19.340997,33.49981 29.011722,50.25 a 6.50065,6.50065 0 0 0 5.628907,3.25 H 228.06641 v -13 H 60.117185 C 50.44646,159.71661 40.776192,142.96698 31.105463,126.2168 a 6.50065,6.50065 0 0 0 -5.6289,-3.25 H 12.999993 V 94.720701 Z M 228.07227,1.1925006e-6 V 13.000002 h 167.94922 l 29.01172,50.249999 a 6.50065,6.50065 0 0 0 5.63086,3.25 h 12.47461 v 28.2461 h 13 v -34.7461 a 6.50065,6.50065 0 0 0 -6.5,-6.499999 H 434.41602 L 405.4043,3.2500012 A 6.50065,6.50065 0 0 0 399.77344,1.1925006e-6 Z m -171.707038,0 A 6.50065,6.50065 0 0 0 50.736325,3.2500012 L 21.724603,53.500002 H 6.4999929 A 6.50065,6.50065 0 0 0 -7.07125e-6,60.000001 v 34.7461 H 12.999993 v -28.2461 h 12.47657 a 6.50065,6.50065 0 0 0 5.6289,-3.25 L 60.119138,13.000002 H 228.06641 V 1.1925006e-6 Z" />
  </g>
</svg>
</template>

<script>
  import { TweenMax } from 'gsap'

  export default {
    name: 'HeaterIcon',
    props: {
      width: {
        type: [Number, String],
        default: 18
      },
      height: {
        type: [Number, String],
        default: 18
      },
      iconColor: {
        type: String,
        default: 'currentColor'
      }
    },
    mounted: function () {
      this.rotateAnim = TweenMax.to(this.$refs.heaterIcon, 0.5, {
          rotation: 360,
          repeat: -1,
          svgOrigin: '500 500',
          ease: 'none',
          paused: true
        })
    },
    methods: {
      startRotate() {
        if (this.rotateAnim.paused() == false) {
          this.rotateAnim.pause()
        } else {
          this.rotateAnim.play()
        }
      },
    },
    data: () => ({
      rotateAnim: null,
    }),
  }
</script>
