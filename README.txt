This project uses a modified version of an implementation of the SHA-256 algorithm to work with Axi Stream on Zynq board which can be found here https://github.com/keanemind/python-sha-256/blob/master/sha256.py.

All you need to run the hardware function in Jupyter notebooks is to place the overlayfiles in the same directory
the notebook is placedin and making sure you have provide the correct path to the overlay function. The notebook
provided shows multiple examples with comments of the code being run with it's output.

As for the vitis test bench, all you need to do is converting your string to ASCII so "Hello" would be the following
array:
{0x68, 0x65, 0x6c, 0x6c, 0x6f}