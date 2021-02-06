const opts = {
  RUN_TARGET: (() => {
    if(/^electron:/.test(process.argv[2])) {
      return "electron"
    }
    return "web"
  })(),
  "ifdef-verbose" : true,
}

let gzip_opt = {
  filename: '[path].gz[query]',
  algorithm: 'gzip',
  include: /\.(js|css|html|svg|json)(\?.*)?$/i,
  minRatio: 0.8,
  deleteOriginalAssets: true,
}

if(/^electron:build/.test(process.argv[2])) {
  gzip_opt = undefined
}

module.exports = {
  "transpileDependencies": [
    "vuetify"
  ],
  pluginOptions: {
    electronBuilder: {
      builderOptions: {
        buildDependenciesFromSource: true,
      },
      nodeIntegration: true,
      externals: ["serialport"],
    },
    compression:{
      gzip: gzip_opt,
    }
  },
  chainWebpack: config => {
    // ifdef-loader
    config.module
    .rule('ifdef-loader')
    .test(/\.(js|vue)$/)
    .use('ifdef-loader')
      .loader('ifdef-loader')
      .options(opts)
      .end()
  }
}
