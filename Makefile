BUILD_DIR=build
include $(N64_INST)/include/n64.mk

src = interviews.c
assets_xm = $(wildcard assets/*.xm)
assets_wav = $(wildcard assets/*.wav)
assets_png = $(wildcard assets/*.png)
assets_m1v = $(wildcard assets/*.m1v)

assets_conv = $(addprefix filesystem/,$(notdir $(assets_xm:%.xm=%.xm64))) \
              $(addprefix filesystem/,$(notdir $(assets_wav:%.wav=%.wav64))) \
              $(addprefix filesystem/,$(notdir $(assets_png:%.png=%.sprite))) \
			  $(addprefix filesystem/,$(notdir $(assets_m1v:%.m1v=%.m1v)))

AUDIOCONV_FLAGS ?= --wav-compress 3 -v
MKSPRITE_FLAGS ?=

all: interviews.z64

filesystem/%.wav64: assets/%.wav
	@mkdir -p $(dir $@)
	@echo "    [SFX] $@"
	$(N64_AUDIOCONV) $(AUDIOCONV_FLAGS) -o $(dir $@) "$<"

filesystem/%.sprite: assets/%.png
	@mkdir -p $(dir $@)
	@echo "    [SPRITE] $@"
	@$(N64_MKSPRITE) $(MKSPRITE_FLAGS) -o filesystem "$<"

filesystem/%.m1v: assets/%.m1v
	@mkdir -p $(dir $@)
	@echo "    [M1V] $@"
	cp "$<" $@

$(BUILD_DIR)/interviews.dfs: $(assets_conv)
$(BUILD_DIR)/interviews.elf: $(src:%.c=$(BUILD_DIR)/%.o)

interviews.z64: N64_ROM_TITLE="N64Brew GameJam 5"
interviews.z64: $(BUILD_DIR)/interviews.dfs 

clean:
	rm -rf $(BUILD_DIR) interviews.z64

-include $(wildcard $(BUILD_DIR)/*.d)

.PHONY: all clean
