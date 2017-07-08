#!/usr/bin/python

IMAGE_RESOLUTION = (640, 480)

IMAGE_BYTE_SIZE = IMAGE_RESOLUTION[0] * IMAGE_RESOLUTION[1] * 3

GRAYSCALE_IMAGE_BYTE_SIZE = IMAGE_RESOLUTION[0] * IMAGE_RESOLUTION[1]

DISCOVER_IMAGE_DATA_SERVICE   = 3068
DISCOVER_RAW_IMAGE_SERVICE    = 3069
RAW_IMAGE_SERVICE             = 3070
IMAGE_DATA_SERVICE            = 3071

SERVICE_DISCOVER_REQUEST_HEADER = "WHERE_IS_"