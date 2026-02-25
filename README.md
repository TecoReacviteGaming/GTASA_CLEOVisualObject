# CLEO Visual Object
A plugin for GTA SA that adds a new Visual Object opcodes that was inspired from CLEOPlus Render Object.

With this plugin:
- Attach many objects as you like to entity.
- Create objects from dff file.
- many more!

## [Opcodes documentation 📝](<https://docs.google.com/document/d/1bE5Vq2hb4EIHE7bmFDZsCSxBfJrubZoy4rd_ADBsdVk/edit?usp=sharing>)
```ini
; Visual Object opcodes
6000=10,%10d% = create_visual_object_to_char_bone %1d% from_dff %2d% bone %3d% offset %4d% %5d% %6d% rotation %7d% %8d% %9d%
6001=4,set_visual_object_scale %1d% scale %2d% %3d% %4d%
6002=1,delete_visual_object %1d%
6003=4,set_visual_object_auto_hide %1d% dead_state %2d% weapon_state %3d% car_state %4d%
6004=2,set_visual_object_visible %1d% visible %2d%
6005=4,set_visual_object_offset %1d% offset %2d% %3d% %4d%
6006=4,set_visual_object_rotation %1d% rotation %2d% %3d% %4d%
6007=5,set_visual_object_distortion %1d% distortion %2d% %3d% %4d% %5d%
6008=2,switch_visual_object_renderer %1d% render_mode %2d%
6009=10,%10d% = create_visual_object_to_char_bone %1d% model_id %2d% bone %3d% offset %4d% %5d% %6d% rotation %7d% %8d% %9d%
600A=9,%9d% = create_visual_object_to_object %1d% model_id %2d% offset %3d% %4d% %5d% rotation %6d% %7d% %8d%
600B=9,%9d% = create_visual_object_to_vehicle %1d% model_id %2d% offset %3d% %4d% %5d% rotation %6d% %7d% %8d%
600C=8,%8d% = create_visual_object_model_id %1d% offset %2d% %3d% %4d% rotation %5d% %6d% %7d%
600D=9,%9d% = create_visual_object_to_object %1d% from_dff %2d% offset %3d% %4d% %5d% rotation %6d% %7d% %8d%
600E=9,%9d% = create_visual_object_to_vehicle %1d% from_dff %2d% offset %3d% %4d% %5d% rotation %6d% %7d% %8d%
600F=8,%8d% = create_visual_object_from_dff %1d% offset %2d% %3d% %4d% rotation %5d% %6d% %7d%
6010=5,set_visual_object %1d% RGBA %2d% %3d% %4d% %5d%
6011=1,is_visual_object_visible %1d%
6012=1,is_visual_object_created %1d%
6013=1,delete_all_visual_objects_from_char %1d% //NOT GLOBAL
6014=1,delete_all_visual_objects_from_vehicle %1d% //NOT GLOBAL
6015=1,delete_all_visual_objects_from_object %1d% //NOT GLOBAL

; Visual Object 1.0.1
6016=2,%2d% = get_visual_object_model_id %1d%
6017=2,%2d% = get_visual_object_dff_name %1d%
6018=2,is_visual_object %1d% model_id %2d%
6019=2,is_visual_object %1d% dff_name %2d%
601A=2,is_visual_object %1d% attached_to_char %2d%
601B=2,is_visual_object %1d% attached_to_vehicle %2d%
601C=2,is_visual_object %1d% attached_to_object %2d%
601D=7,store_coords_to %5d% %6d% %7d% from_visual_object %1d% with_offset %2d% %3d% %4d%
```

## License 📜
```txt
MIT License

Copyright (c) 2026 TecoReacvite

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```