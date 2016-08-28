# Runfragment
runfragment is a standalone utility that replicates the rendering functionality of Shadertoy.com and GLSLSandbox.com. It also keeps track of file changes and automaticly applies them.
## Usage
### Run
You can run it for a single fragment shader file:
```
runfragment Image.glsl
```
or
```
runfragment --Image Image.glsl
```

By default runfragment uses GLSL Sandbox uniforms format, you can change it with `--format shadertoy` (`-f s`) option.

Now you can edit the shader using your favorite text editor and it will be reloaded on save.

### Channels
You can set channel for Image (main) shader or for Buf (buffer) in that manner:
```
runfragment --format shadertoy --Image Image.glsl --Image-c0 tex.png
```

In the same way as in Shadertoy a channel can be set as render buffer:
```
runfragment --format shadertoy --Image Image.glsl --BufA BufA.glsl --Image-c0 BufA
```

### Config
If you tired to type all this arguments, you can use config file:
```
runfragment -c config.ini
```
config.ini:
```ini
format = shadertoy

Image = Image.glsl
Image-c0 BufA

BufA = BufA.glsl
```

### Downloading
It also has ```--download``` option which can be used to download a shader from shadertoy.com (creates directory with a config file and other files) and glslsandbox.com (creates a single shader file).
```
runfragment --download https://www.shadertoy.com/view/MdX3zr
```

## Not implimented features
 - Keyboard
 - iResolution and (probably?) several other uniforms
 - Sound functionality
 - Video functionality

## License
MIT
