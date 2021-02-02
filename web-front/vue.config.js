module.exports = {
  "transpileDependencies": [
    "vuetify"
  ],
  pluginOptions: {
    compression:{
		  gzip: {
			  filename: '[path].gz[query]',
			  algorithm: 'gzip',
			  include: /\.(js|css|html|svg|json)(\?.*)?$/i,
			  minRatio: 0.8,
			  deleteOriginalAssets: true,
		  }
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
