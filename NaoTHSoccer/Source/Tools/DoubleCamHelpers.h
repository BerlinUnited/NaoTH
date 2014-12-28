#ifndef _DoubleCamHelpers_h_
#define _DoubleCamHelpers_h_

// usefull code snipsets and macros

#define DOUBLE_CAM_REQUIRE(module,name)      \
  const name& get##name() const {            \
    if(cameraID == CameraInfo::Top) {        \
      return module##Base::get##name##Top(); \
    } else {                                 \
      return module##Base::get##name();      \
    }                                        \
  }

#define DOUBLE_CAM_PROVIDE(module,name)      \
  name& get##name() const {                  \
    if(cameraID == CameraInfo::Top) {        \
      return module##Base::get##name##Top(); \
    } else {                                 \
      return module##Base::get##name();      \
    }                                        \
  }

#endif // _DoubleCamHelpers_h_