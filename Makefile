.PHONY: all install run test clean setup

all: install

install:
	pip install -r requirements.txt

run:
	python src/rigol_gui.py

test:
	python src/test_components.py

clean:
	find . -type d -name __pycache__ -exec rm -rf {} +

setup:
	./setup.sh