[![](glTF.png)](https://github.com/KhronosGroup/glTF/tree/master/specification/2.0)

# Download glTF 2.0 content from wayfair

wayfair2gltf2 is a command line tool for downloading glTF assets by [SKU](https://en.wikipedia.org/wiki/Stock_keeping_unit) from [wayfair](https://www.wayfair.com/).  
It uses the [Wayfair's Realtime 3d Model API](https://www.aboutwayfair.com/tech-blog/welcome-to-wayfairs-realtime-3d-model-api).

Usage: `wayfair2gltf2.exe [-e email -p password -s sku -v false]`  

`-e email` By default, no email and the demo server is used.  
`-p password` By default, no password and the demo server is used.  
`-s sku` By default, the ZPCD5744 is used. For registered users, the FV50959 is used.  
`-v false` Verbose mode for curl.  


## Software Requirements

* C/C++ 17 compiler e.g. gcc
* [Eclipse IDE for C/C++ Developers](https://www.eclipse.org/downloads/packages/release/2021-03/r/eclipse-ide-cc-developers)


## Import the generated glTF

Import the generated glTF in e.g. [Gestaltor](https://gestaltor.io/) and compose a new scene.  

A short tutorial can be found here: [Gestaltor - How to compose a scene from multiple glTFs using the navigation cube](https://docs.gestaltor.io/#compose-a-scene-from-multiple-gltfs-using-the-navigation-cube).  
