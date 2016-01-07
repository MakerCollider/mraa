{
  "name" : "mraa-makercollider",
  "description": "IO library that helps you use I2c, SPI, gpio, uart, pwm, analog inputs (aio) and more on Intel Edison platform and new breakout extension board which is designed by the MakerCollider",
  "keywords":["gpio", "edison", "io", "mraajs", "spi", "i2c", "intel"],
  "homepage": "https://github.com/MakerCollider/mraa",
  "main" : "./mraa.node",
  "engines": {
    "node": ">= 0.10.x"
  },
  "bugs": {
    "url" : "https://github.com/MakerCollider/mraa/issues"
  },
  "postinstall": {
    "./install_edisontools.sh"
  }
  "license": "MIT",
  "version": "@VERSION@",
  "authors": "See https://github.com/MakerCollider/mraa/graphs/contributors"
}
