#*****************************************************************************
# * Copyright ©2019. Radiocrafts AS (Radiocrafts).  All Rights Reserved. 
# * Permission to use, copy, modify, and distribute this software and 
# * its documentation, without fee and without a signed licensing 
# * agreement, is hereby granted, provided that the above copyright 
# * notice, this paragraph and the following two paragraphs appear in 
# * all copies, modifications, and distributions.
# * 
# * IN NO EVENT SHALL RADIOCRFTS BE LIABLE TO ANY PARTY FOR DIRECT, 
# * INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING 
# * LOST PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS 
# * DOCUMENTATION, EVEN IF RADIOCRAFTS HAS BEEN ADVISED OF THE 
# * POSSIBILITY OF SUCH DAMAGE. 
# * 
# * RADIOCRAFTS SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT 
# * NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND 
# * FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE AND ACCOMPANYING 
# * DOCUMENTATION, IF ANY, PROVIDED HEREUNDER IS PROVIDED "AS IS". 
# * RADIOCRAFTS HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, 
# * UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
#****************************************************************************

# Default values. You can override them at the command line like for
# instance this if you are running on a windows computer:
# make uploadImage PORT=COM3

SOURCE_FILE?=SRC/My_App.c
OUTPUT_FILENAME?=Output/$(notdir $(basename $(SOURCE_FILE)))
PORT=/dev/ttyUSB0
AKEY=Application.key
NKEY=Network.key

Framework_Path=../../Framework

include $(Framework_Path)/Tools/RIIM.mk