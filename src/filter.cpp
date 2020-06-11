// void setA(float val)
// {
// 	setFilterAmount(val);
// 	filterMode = LOWPASS;
// }

// void setB(float val)
// {
// 	setFilterAmount(val);
// 	filterMode = HIPASS;
// }

// void setC(float val)
// {
// 	if (val > 0.5)
// 	{
		// setFilterAmount((val - 0.5) * 2);
// 		filterMode = HIPASS;
// 	}
// 	else
// 	{
// 		setFilterAmount((0.5 - val) * 2);
// 		filterMode = LOWPASS;
// 	}
// }
// void initFilter(float val)
// {
// }

// void mapFilterKeys()
// {
// 	//setPot0ChangedEvent(&setA);
// 	//setPot1ChangedEvent(&setB);
// 	if (adc3 > 400)
// 	{
// 		setPotChangedEvent(4, &initFilter);
// 		setPotChangedEvent(0, &setC);
// 	}
// 	else if (adc3 < 1200)
// 	{
// 		setPotChangedEvent(0, &initFilter);
// 		setPotChangedEvent(4, &setC);
// 	}

// 	setC(0.25);
// }
