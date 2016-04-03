#include <mruby.h>
#include <mruby/array.h>
#include <mruby/data.h>
#include <mruby/variable.h>
#include <mruby/string.h>

#include <gamepad/Gamepad.h>
#include <gamepad/Gamepad_private.c>

#ifdef _WIN32
#include <gamepad/Gamepad_windows_dinput.c>
#elif __APPLE__
#include <gamepad/Gamepad_macosx.c>
#else
#include <gamepad/Gamepad_linux.c>
#endif


static mrb_value create_device(mrb_state* mrb, struct Gamepad_device* pad)
{
  mrb_value axes = mrb_ary_new_capa(mrb, pad->numAxes);
  for (unsigned int i=0; i < pad->numAxes; ++i) {
    mrb_value f = mrb_float_value(mrb, pad->axisStates[i]);
    mrb_ary_push(mrb, axes, f);
  }
  mrb_value buttons = mrb_ary_new_capa(mrb, pad->numButtons);
  for (unsigned int i=0; i < pad->numButtons; ++i) {
    mrb_value b = mrb_bool_value(pad->buttonStates[i]);
    mrb_ary_push(mrb, buttons, b);
  }

  mrb_value v;
  v = mrb_funcall(
      mrb,mrb_obj_value(mrb_class_get_under(mrb, mrb_module_get(mrb, "Gamepad"), "Gamepad_device")),
      "new",
      8,
      mrb_fixnum_value(pad->deviceID),
      mrb_str_new_cstr(mrb, pad->description),
      mrb_fixnum_value(pad->vendorID),
      mrb_fixnum_value(pad->productID),
      mrb_fixnum_value(pad->numAxes),
      mrb_fixnum_value(pad->numButtons),
      axes,
      buttons);
  return v;
}

static mrb_value _init(mrb_state* mrb, mrb_value self)
{
  Gamepad_init();
  return mrb_nil_value();
}

static mrb_value _shutdown(mrb_state* mrb, mrb_value self)
{
  Gamepad_shutdown();
  return mrb_nil_value();
}

static mrb_value num_devices(mrb_state* mrb, mrb_value self)
{
  return mrb_fixnum_value(Gamepad_numDevices());
}

static mrb_value device_at_index(mrb_state* mrb, mrb_value self)
{
  mrb_int i;
  mrb_get_args(mrb, "i", &i);
  struct Gamepad_device* pad = Gamepad_deviceAtIndex(i);
  if (pad) return create_device(mrb, pad);
  else return mrb_nil_value();
}

static mrb_value detect_devices(mrb_state* mrb, mrb_value self)
{
  Gamepad_detectDevices();
  return mrb_nil_value();
}

static mrb_value process_events(mrb_state* mrb, mrb_value self)
{
  Gamepad_processEvents();
  return mrb_nil_value();
}

static void cb_attach(struct Gamepad_device* device, void* context)
{
  mrb_state* mrb = (mrb_state*)context;
  mrb_value blk = mrb_mod_cv_get(mrb, mrb_module_get(mrb, "Gamepad"), mrb_intern_lit(mrb, "@@blk_attach"));
  if (!mrb_nil_p(blk)) {
    mrb_value v = create_device(mrb, device);
    mrb_yield_argv(mrb, blk, 1, &v);
  }
}

static void cb_remove(struct Gamepad_device* device, void* context)
{
  mrb_state* mrb = (mrb_state*)context;
  mrb_value blk = mrb_mod_cv_get(mrb, mrb_module_get(mrb, "Gamepad"), mrb_intern_lit(mrb, "@@blk_remove"));
  if (!mrb_nil_p(blk)) {
    mrb_value v = create_device(mrb, device);
    mrb_yield_argv(mrb, blk, 1, &v);
  }
}

static void cb_button_down(struct Gamepad_device* device, unsigned int buttonID, double timestamp, void* context)
{
  mrb_state* mrb = (mrb_state*)context;
  mrb_value blk = mrb_mod_cv_get(mrb, mrb_module_get(mrb, "Gamepad"), mrb_intern_lit(mrb, "@@blk_button_down"));
  if (!mrb_nil_p(blk)) {
    mrb_value argv[3];
    argv[0] = create_device(mrb, device);
    argv[1] = mrb_fixnum_value(buttonID);
    argv[2] = mrb_float_value(mrb, timestamp);
    mrb_yield_argv(mrb, blk, 3, argv);
  }
}

static void cb_button_up(struct Gamepad_device* device, unsigned int buttonID, double timestamp, void* context)
{
  mrb_state* mrb = (mrb_state*)context;
  mrb_value blk = mrb_mod_cv_get(mrb, mrb_module_get(mrb, "Gamepad"), mrb_intern_lit(mrb, "@@blk_button_up"));
  if (!mrb_nil_p(blk)) {
    mrb_value argv[3];
    argv[0] = create_device(mrb, device);
    argv[1] = mrb_fixnum_value(buttonID);
    argv[2] = mrb_float_value(mrb, timestamp);
    mrb_yield_argv(mrb, blk, 3, argv);
  }
}

static void cb_axis(struct Gamepad_device* device, unsigned int axisID, float value, float last_value, double timestamp, void* context)
{
  mrb_state* mrb = (mrb_state*)context;
  mrb_value blk = mrb_mod_cv_get(mrb, mrb_module_get(mrb, "Gamepad"), mrb_intern_lit(mrb, "@@blk_axis"));
  if (!mrb_nil_p(blk)) {
    mrb_value argv[5];
    argv[0] = create_device(mrb, device);
    argv[1] = mrb_fixnum_value(axisID);
    argv[2] = mrb_float_value(mrb, value);
    argv[3] = mrb_float_value(mrb, last_value);
    argv[4] = mrb_float_value(mrb, timestamp);
    mrb_yield_argv(mrb, blk, 5, argv);
  }
}


void mrb_mruby_gamepad_gem_init(mrb_state* mrb)
{
  struct RClass* cls = mrb_define_module(mrb, "Gamepad");

  mrb_define_module_function(mrb, cls, "init", _init, MRB_ARGS_NONE());
  mrb_define_module_function(mrb, cls, "shutdown", _shutdown, MRB_ARGS_NONE());
  mrb_define_module_function(mrb, cls, "numDevices", num_devices, MRB_ARGS_NONE());
  mrb_define_module_function(mrb, cls, "deviceAtIndex", device_at_index, MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, cls, "detectDevices", detect_devices, MRB_ARGS_NONE());
  mrb_define_module_function(mrb, cls, "processEvents", process_events, MRB_ARGS_NONE());

  Gamepad_deviceAttachFunc(cb_attach, mrb);
  Gamepad_deviceRemoveFunc(cb_remove, mrb);
  Gamepad_buttonDownFunc(cb_button_down, mrb);
  Gamepad_buttonUpFunc(cb_button_up, mrb);
  Gamepad_axisMoveFunc(cb_axis, mrb);
}


void mrb_mruby_gamepad_gem_final(mrb_state* mrb)
{
}
