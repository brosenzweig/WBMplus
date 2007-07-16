include ../common.mk
INC    = ../include
SRC    = ../src
OBJ    = ../obj
BIN    = ../bin
CC     = gcc -g -Wall -I$(INC) -I../../CMlib/include -I../../MFlib/include
LINK   = gcc
LIBS   = -L../../CMlib/lib -L../../MFlib/lib -lMF10 -lCM10 -lm
TARGET = $(BIN)/wbmplus.bin

INCLUDES = $(INC)/MD.h

OBJS = $(OBJ)/WBMMain.o\
       $(OBJ)/MDAccBalance.o\
       $(OBJ)/MDAccumulate.o\
       $(OBJ)/MDAvgNSteps.o\
       $(OBJ)/MDBaseFlow.o\
       $(OBJ)/MDCParams.o\
       $(OBJ)/MDDischarge.o\
       $(OBJ)/MDDischAccumulate.o\
       $(OBJ)/MDDischMuskingum.o\
       $(OBJ)/MDDischCascade.o\
       $(OBJ)/MDLandCover.o\
       $(OBJ)/MDLSAreaIndex.o\
       $(OBJ)/MDHamonReferenceETP.o\
       $(OBJ)/MDInfiltration.o\
       $(OBJ)/MDIntercept.o\
       $(OBJ)/MDIrrigation.o\
       $(OBJ)/MDGrossRad.o\
       $(OBJ)/MDPrecipitation.o\
       $(OBJ)/MDPotET.o\
       $(OBJ)/MDPotETHamon.o\
       $(OBJ)/MDPotETJensen.o\
       $(OBJ)/MDPotETPstd.o\
       $(OBJ)/MDPotETPsTaylor.o\
       $(OBJ)/MDPotETPMday.o\
       $(OBJ)/MDPotETPMdn.o\
       $(OBJ)/MDPotETSWGday.o\
       $(OBJ)/MDPotETSWGdn.o\
       $(OBJ)/MDPotETTurc.o\
       $(OBJ)/MDPotETlib.o\
       $(OBJ)/MDRefDischarge.o\
       $(OBJ)/MDReferenceETP.o\
       $(OBJ)/MDRelHumidity.o\
       $(OBJ)/MDReservoirs.o\
       $(OBJ)/MDRunoff.o\
       $(OBJ)/MDSMoistChg.o\
       $(OBJ)/MDSPackChg.o\
       $(OBJ)/MDSoilProp.o\
       $(OBJ)/MDRootDepth.o\
       $(OBJ)/MDSolarRad.o\
       $(OBJ)/MDWaterBalance.o\
       $(OBJ)/MDWaterSurplus.o\
       $(OBJ)/MDWetDays.o\
       $(OBJ)/MDWind.o\
       $(OBJ)/MDBgc.o

all: ${TARGET}

clean:
	rm $(TARGET) $(OBJS)

$(TARGET): $(OBJS) $(SRC)/Makefile
	$(LINK) $(OBJS) $(LIBS) -o $(TARGET)

$(OBJ)/%.o: $(SRC)/%.c $(INCLUDES)
	$(CC) -o $(OBJ)/$*.o -c $(SRC)/$*.c
