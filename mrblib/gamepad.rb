module Gamepad
  @@_installed = false
  @@blk_attach = nil
  def Gamepad.deviceAttachFunc(&blk)
    @@blk_attach = blk if block_given?
  end
  @@blk_remove = nil
  def Gamepad.deviceRemoveFunc(&blk)
    @@blk_remove = blk if block_given?
  end
  @@blk_button_down = nil
  def Gamepad.buttonDownFunc(&blk)
    @@blk_button_down = blk if block_given?
  end
  @@blk_button_up = nil
  def Gamepad.buttonUpFunc(&blk)
    @@blk_button_up = blk if block_given?
  end
  @@blk_axis = nil
  def Gamepad.axisMoveFunc(&blk)
    @@blk_axis = blk if block_given?
  end

  class Gamepad_device
    attr_reader :deviceID, :description, :vendorID, :productID, :numAxes, :numButtons, :axisStates, :buttonStates
    def initialize(deviceID, description, vendorID, productID, numAxes, numButtons, axisStates, buttonStates)
      @deviceID = deviceID
      @description = description
      @vendorID = vendorID
      @productID = productID
      @numAxes = numAxes
      @numButtons = numButtons
      @axisStates = axisStates
      @buttonStates = buttonStates
    end
  end
end
