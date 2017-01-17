BUILD_DIR=.build

all:
	$(shell [[ ! -d $(BUILD_DIR) ]] && mkdir $(BUILD_DIR) )
	cd $(BUILD_DIR) ; cmake .. ; make ; cd ..
	cp $(BUILD_DIR)/packimages .

clean:
	rm -fr $(BUILD_DIR) packimages

