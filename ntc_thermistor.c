#include <stdio.h>
 
/* ------------------ CONSTANTS ------------------ */
#define ADC_MIN     0
#define ADC_MAX     1023
 
#define TEMP_0C_RES     30000.0f
#define TEMP_25C_RES    10000.0f
#define TEMP_50C_RES    4000.0f
 
/* ================= VALIDATION ================= */
int isValidADCConfig(unsigned int adc_value,
                     float reference_voltage,
                     float fixed_resistor)
{
    /* ADC value check */
    if (adc_value <0 || adc_value > 1023)
        return 0;
 
    /* Reference voltage check */
    if (reference_voltage <= 0.0f || reference_voltage > 5.0f)
        return 0;
 
    /* Fixed resistor value check */
    if (fixed_resistor < 1000.0f || fixed_resistor > 100000.0f)
        return 0;
 
    return 1;   /* all valid */
}
 
 
/* ------------------ PART 1 ------------------
   Convert ADC value to voltage
------------------------------------------------ */
float convert_adc_to_voltage(unsigned int raw_adc_value, float vref)
{
    return (raw_adc_value / 1023.0f) * vref;
}
 
/* ------------------ PART 2 ------------------
   Convert voltage to temperature
------------------------------------------------ */
float convert_voltage_to_temperature(float thermistor_voltage,
                                     float fixed_resistor_ohms,
                                     float vref_divider)
{
    float r_thermistor;
    float temperature;
 
    /* Voltage divider formula */
    r_thermistor = fixed_resistor_ohms *
                   (thermistor_voltage /
                   (vref_divider - thermistor_voltage));
 
    /* Piecewise linear interpolation */
 
    /* Between 0°C and 25°C */
    if (r_thermistor >= TEMP_25C_RES)
    {
        temperature = 25.0f *
                      (TEMP_0C_RES - r_thermistor) /
                      (TEMP_0C_RES - TEMP_25C_RES);
    }
    /* Between 25°C and 50°C */
    else if (r_thermistor >= TEMP_50C_RES)
    {
        temperature = 25.0f +
                      25.0f *
                      (TEMP_25C_RES - r_thermistor) /
                      (TEMP_25C_RES - TEMP_50C_RES);
    }
    else
    {
        temperature = 50.0f;   /* Above 50°C */
    }
 
    return temperature;
}
 
/* ------------------ MAIN FUNCTION ------------------ */
int main(void)
{
    unsigned int adc_value;
    float vref;
    float fixed_resistor;
    float voltage;
    float temperature;
 
    while (1) // Infinite loop to keep program running
    {
        /* User input */
        printf("\nEnter ADC value (0 - 1023): ");
        scanf("%u", &adc_value);
 
        printf("Enter ADC reference voltage (V): ");
        scanf("%f", &vref);
 
        printf("Enter fixed resistor value (Ohms): ");
        scanf("%f", &fixed_resistor);
 
        /* Validation after all inputs */
        if (!isValidADCConfig(adc_value, vref, fixed_resistor))
        {
            printf("❌ Invalid input values!\n");
            printf("ADC: 0–1023 | Vref: 0–5V | Resistor: 1k–100k\n");
            continue; // Ask user to enter again
        }
 
        /* Part 1: ADC to Voltage */
        voltage = convert_adc_to_voltage(adc_value, vref);
 
        /* Part 2: Voltage to Temperature */
        temperature = convert_voltage_to_temperature(voltage,
                                                     fixed_resistor
                                                     ,vref);
 
        /* Output */
        printf("\n----- Results -----\n");
        printf("ADC Value        : %u\n", adc_value);
        printf("Voltage (V)      : %.3f V\n", voltage);
        printf("Temperature (°C) : %.2f °C\n", temperature);
    }
 
    return 0;
}
 