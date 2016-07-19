{
  "name" : "mraa",
  "description": "IO library that helps you use I2c, SPI, gpio, uart, pwm, analog inputs (aio) and more on a number of platforms such as the Intel galileo, the Intel edison and others",
  "keywords":["gpio", "edison","galileo","io", "mraajs", "spi", "i2c", "minnow", "intel", "firmata"],
  "homepage": "https://github.com/MakerCollider/mraa",
  "main" : "./mraa.node",
  "engines": {
    "node": ">= 0.10.x"
  },
  "bugs": {
    "url" : "https://github.com/MakerCollider/mraa/issues"
  },
  "scripts": {
    "postinstall": "./install_edisontools.sh"
  },
  "license": "MIT",
  "version": "@VERSION@",
  "authors": "See https://github.com/MakerCollider/mraa/graphs/contributors"
}
