.PHONY: clean All

All:
	@echo "----------Building project:[ PCAM - Release ]----------"
	@cd "PCAM" && "$(MAKE)" -f  "PCAM.mk"
clean:
	@echo "----------Cleaning project:[ PCAM - Release ]----------"
	@cd "PCAM" && "$(MAKE)" -f  "PCAM.mk" clean
