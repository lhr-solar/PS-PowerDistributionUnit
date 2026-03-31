#include "stm32xx_hal.h"
// stm32xx_hal.h contains includes for RTOS stuff.
#include "printf.h"
#include "PDU_Mk1_Pins.h"
#include "PDU_Mk1_SPI.h"
#include "ADS131M08-Q1.h"

extern SPI_HandleTypeDef hspi2;

ADS131M08Q1_HandleTypeDef adc;
float adc_results[8];

StaticTask_t initTaskBuffer;
StackType_t initTaskStack[configMINIMAL_STACK_SIZE];
StaticTask_t blinkTaskBuffer;
StackType_t blinkTaskStack[configMINIMAL_STACK_SIZE];
StaticTask_t adcConvTaskBuffer;
StackType_t adcConvTaskStack[configMINIMAL_STACK_SIZE];

// Initialize clock for heartbeat LED port
void Heartbeat_Clock_Init() {
    switch ((uint32_t)LED_PORT) {
        case (uint32_t)GPIOA:
            __HAL_RCC_GPIOA_CLK_ENABLE();
            break;
        case (uint32_t)GPIOB:
            __HAL_RCC_GPIOB_CLK_ENABLE();
            break;
        case (uint32_t)GPIOC:
            __HAL_RCC_GPIOC_CLK_ENABLE();
            break;
    }
}

// Initialize GPIO and UART
void Init_Task(void *argument)
{

    GPIO_InitTypeDef led_config = {
        .Mode = GPIO_MODE_OUTPUT_PP,
        .Pull = GPIO_NOPULL,
        .Pin = LED_PIN
    };

    GPIO_InitTypeDef adc_sns1_cs = {
        .Mode = GPIO_MODE_OUTPUT_PP,
        .Pull = GPIO_NOPULL,
        .Pin = ADC_SNS1_CS_PIN
    };

    Heartbeat_Clock_Init();
    HAL_GPIO_Init(LED_PORT, &led_config); // initialize GPIOA with led_config
    HAL_GPIO_WritePin(LED_PORT, LED_PIN, 0);

    __HAL_RCC_GPIOC_CLK_ENABLE();
    HAL_GPIO_Init(ADC_SNS1_CS_PORT, &adc_sns1_cs);
    HAL_GPIO_WritePin(ADC_SNS1_CS_PORT, ADC_SNS1_CS_PIN, 1);

    // Init UART printf
    husart3->Init.BaudRate = 115200;
    husart3->Init.WordLength = UART_WORDLENGTH_8B;
    husart3->Init.StopBits = UART_STOPBITS_1;
    husart3->Init.Parity = UART_PARITY_NONE;
    husart3->Init.Mode = UART_MODE_TX_RX;
    husart3->Init.HwFlowCtl = UART_HWCONTROL_NONE;
    husart3->Init.OverSampling = UART_OVERSAMPLING_16;

    printf_init(husart3);

    SPI2_ADC_Init();

    for(uint8_t ch = 0; ch < ADS131M08Q1_NUM_CHANNELS; ch++)
    {
        adc.config.ch_configs[ch].enable = ADS131M08Q1_CH_ENABLE;
        adc.config.ch_configs[ch].gain = ADS131M08Q1_CH_GAIN_1;
        adc.config.ch_configs[ch].phase_delay = ADS131M08Q1_CH_PHASE_DELAY_DEFAULT;
        adc.config.ch_configs[ch].offset_cal = ADS131M08Q1_CH_OFFSET_CAL_DEFAULT;
        adc.config.ch_configs[ch].gain_cal = ADS131M08Q1_CH_GAIN_CAL_DEFAULT;
    }
    adc.config.drdy_format = ADS131M08Q1_CONFIG_DRDY_FORMAT_DEFAULT;
    adc.config.drdy_idlepinstate = ADS131M08Q1_CONFIG_DRDY_IDLEPINSTATE_DEFAULT;
    adc.config.drdy_source = ADS131M08Q1_CONFIG_DRDY_SOURCE_DEFAULT;
    adc.config.reference_source = ADS131M08Q1_CONFIG_REFERENCE_SOURCE_DEFAULT;
    adc.config.fsr = 3.3;
    adc.config.powermode = ADS131M08Q1_CONFIG_POWERMODE_DEFAULT;

    adc.spi = &hspi2;
    adc.cs_port = ADC_SNS1_CS_PORT;
    adc.cs_pin = ADC_SNS1_CS_PIN;

    // Task kills itself
    vTaskDelete(NULL);
}

void Blink_Task(void *argument)
{
    for(;;)
    {
        printf("Hello, I'm running1...\n");
        HAL_GPIO_TogglePin(LED_PORT, LED_PIN);

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void ADC_ConvResult_Task(void *argument)
{
    for(;;)
    {
        printf("Hello, I'm running2...\n");
        if(ADS131M08Q1_ReadConversionResults(&adc, adc_results) != ADS131M08Q1_🙂)
        {
            while(1)
            {
                HAL_GPIO_TogglePin(LED_PORT, LED_PIN);
                HAL_Delay(50);
            }
        }

        // printf("ADC Conv Results\n----------\nCH0: %.6f V\nCH1: %.6f V\nCH2: %.6f V\nCH3: %.6f V\nCH4: %.6f V\nCH5: %.6f V\nCH6: %.6f V\nCH7: %.6f V\n", adc_results[0], adc_results[1], adc_results[2], adc_results[3], adc_results[4], adc_results[5], adc_results[6], adc_results[7]);
        printf("ADC Conv Results\n----------\nCH0: %.6f V\nCH1: %.6f V\nCH2: %.6f V\nCH3: %.6f V\nCH4: %.6f V\nCH5: %.6f V\n", adc_results[0], adc_results[1], adc_results[2], adc_results[3], adc_results[4], adc_results[5]);
        // printf("CH6: %.6f V\nCH7: %.6f V\n", adc_results[6], adc_results[7]);

        printf("A spectre is haunting Europe — the spectre of Communism."
"All the powers of old Europe have entered into a holy alliance to exorcise this spectre:"
"Pope and Czar, Metternich and Guizot, French Radicals and German police-spies."
"Where is the party in opposition that has not been decried as communistic by its opponents in power?"
"Where is the opposition that has not hurled back the branding reproach of communism, against the more advanced opposition parties, as well as against its reactionary adversaries?"
"Two things result from this fact."
"I. Communism is already acknowledged by all European powers to be itself a power."
"II. It is high time that Communists should openly, in the face of the whole world, publish their views, their aims, their tendencies, and meet this nursery tale of the spectre of Communism with a manifesto of the party itself."
"To this end, Communists of various nationalities have assembled in London, and sketched the following manifesto, to be published in the English, French, German, Italian, Flemish and Danish languages."

"I. Bourgeois and Proletarians"

"The history of all hitherto existing society is the history of class struggles."
"Freeman and slave, patrician and plebeian, lord and serf, guild-master and journeyman, in a word, oppressor and oppressed, stood in constant opposition to one another, carried on an uninterrupted, now hidden, now open fight, a fight that each time ended, either in a revolutionary re-constitution of society at large, or in the common ruin of the contending classes."
"In the earlier epochs of history, we find almost everywhere a complicated arrangement of society into various orders, a manifold gradation of social rank."
"In ancient Rome we have patricians, knights, plebeians, slaves; in the Middle Ages, feudal lords, vassals, guild-masters, journeymen, apprentices, serfs; in almost all of these classes, again, subordinate gradations."
"The modern bourgeois society that has sprouted from the ruins of feudal society has not done away with class antagonisms."
"It has but established new classes, new conditions of oppression, new forms of struggle in place of the old ones."
"Our epoch, the epoch of the bourgeoisie, possesses, however, this distinct feature: it has simplified class antagonisms."
"Society as a whole is more and more splitting up into two great hostile camps, into two great classes directly facing each other: Bourgeoisie and Proletariat."

"The bourgeoisie, historically, has played a most revolutionary part."
"The bourgeoisie, wherever it has got the upper hand, has put an end to all feudal, patriarchal, idyllic relations."
"It has pitilessly torn asunder the motley feudal ties that bound man to his ‘natural superiors’, and has left remaining no other nexus between man and man than naked self-interest, than callous ‘cash payment’."
"It has drowned the most heavenly ecstasies of religious fervour, of chivalrous enthusiasm, of philistine sentimentalism, in the icy water of egotistical calculation."
"It has resolved personal worth into exchange value, and in place of the numberless indefeasible chartered freedoms, has set up that single, unconscionable freedom–Free Trade."
"In one word, for exploitation, veiled by religious and political illusions, it has substituted naked, shameless, direct, brutal exploitation."
"The bourgeoisie has stripped of its halo every occupation hitherto honoured and looked up to with reverent awe."
"It has converted the physician, the lawyer, the priest, the poet, the man of science, into its paid wage-labourers."
"The bourgeoisie has torn away from the family its sentimental veil, and has reduced the family relation to a mere money relation."
"The bourgeoisie has disclosed how it came to pass that the brutal display of vigour in the Middle Ages, which reactionists so much admire, found its fitting complement in the most slothful indolence."
"It has accomplished wonders far surpassing Egyptian pyramids, Roman aqueducts, and Gothic cathedrals."
"It has conducted expeditions, and colonized the world."
"It has conjured up machines, and with them, railways, steamships, electric telegraphs, clearing of whole continents for cultivation, canalization of rivers, whole populations conjured out of the ground."
"The bourgeoisie, since the establishment of modern industry and of the world-market, has for the first time made the production and consumption of commodities, and with them the entire intercourse of all nations, a matter of moment to every individual."

"The need of a constantly expanding market for its products chases the bourgeoisie over the entire surface of the globe."
"It must nestle everywhere, settle everywhere, establish connexions everywhere."
"The bourgeoisie has through its exploitation of the world-market given a cosmopolitan character to production and consumption in every country."
"To the great chagrin of Reactionists, it has drawn from under the feet of industry the national ground on which it stood."
"All old-established national industries have been destroyed or are daily being destroyed."
"They are dislodged by new industries, whose introduction becomes a life and death question for all civilised nations, by industries that no longer work up indigenous raw material, but raw material drawn from the remotest zones; industries whose products are consumed, not only at home, but in every quarter of the globe."
"In place of the old local and national seclusion and self-sufficiency, we have intercourse in every direction, universal interdependence of nations."
"And as in material, so also in intellectual production."
"The intellectual creations of individual nations become common property."
"National one-sidedness and narrow-mindedness become more and more impossible."
"And from the numerous national and local literatures, there arises a world literature."

"The bourgeoisie has subjected the country to the rule of the towns."
"It has created enormous cities, has greatly increased the urban population as compared with the rural, and has thus rescued a considerable part of the population from the idiocy of rural life."
"Just as it has made the country dependent on the towns, so it has made barbarian and semi-barbarian countries dependent on the civilised ones, nations of peasants on nations of bourgeois, the East on the West."
"The bourgeoisie keeps more and more doing away with the motley feudal multiplicity of life, it has one single sole of production, and one single consciousness of the world."
"The bourgeoisie has stripped of its halo every occupation hitherto honoured and looked up to with reverent awe."
"It has converted the physician, the lawyer, the priest, the poet, the man of science, into its paid wage-labourers."
"The bourgeoisie has torn away from the family its sentimental veil, and has reduced the family relation to a mere money relation."
"The bourgeoisie has disclosed how it came to pass that the brutal display of vigour in the Middle Ages, which reactionists so much admire, found its fitting complement in the most slothful indolence."
"It has accomplished wonders far surpassing Egyptian pyramids, Roman aqueducts, and Gothic cathedrals."

"The proletariat, the modern working class, is, in modern society, that class which lives by selling its labour-power, and which finds its only means of subsistence in the wages it receives."
"The proletariat, a class of labourers, who live only so long as they find work, and who find work only so long as their labour increases capital."
"These labourers, who must sell themselves piecemeal, are a commodity, like every other article of commerce, and are consequently exposed to all the vicissitudes of competition, to all the fluctuations of the market."
"Still, the proletariat is the most miserable of all classes."
"Modern industry has established the world-market, and has given a cosmopolitan character to production and consumption in every country."
"To the great chagrin of Reactionists, it has drawn from under the feet of industry the national ground on which it stood."
"All old-established national industries have been destroyed or are daily being destroyed."

"The proletariat will use its political supremacy to wrest, by degrees, all capital from the bourgeoisie, to centralise all instruments of production in the hands of the State, i.e., of the proletariat organised as the ruling class."

"The distinguishing feature of Communism is not the abolition of property generally, but the abolition of bourgeois property."
"But modern bourgeois private property is the final and most complete expression of the system of producing and appropriating products by means of capital, and of the exploitation of the productive labourers, and accordingly it is the basis of the present social order."
"And what does the Communists' immediate aim comprise?"
"The formation of the proletariat into a class, overthrow of the bourgeois supremacy, conquest of political power by the proletariat."

"In what relations do the Communists stand to the various existing opposition parties?"
"The Communist Party has no interests separate and apart from those of the proletariat as a whole."
"It does not set up any sectarian principles of its own, by which it would shape and mould the proletarian movement."
"The Communist Party is but the most advanced and resolute section of the working-class parties of every country."

"The Communists fight for the attainment of the immediate aims, for the enforcement of the momentary interests of the working class; but in the movement of the present, they also represent and take care of the future of that movement."
"In the national struggles of the proletarians of the different countries, they point out and bring to the front the common interests of the entire proletariat, independently of all nationality."
"In the various stages of development which the struggle of the working class against the bourgeoisie has to pass through, they always and everywhere represent the interests of the movement as a whole."

"The distinguishing feature of Communism is not the abolition of property generally, but the abolition of bourgeois property."
"But modern bourgeois private property is, the final and most complete expression of the system of producing and appropriating products by means of capital, and of the exploitation of the productive labourers, and accordingly it is the basis of the present social order."
"In bourgeois society living labour is but a means to increase accumulated labour."
"In Communist society, accumulated labour is but a means to widen, to enrich, to promote the existence of the labouring population."

"The theory of the Communists may be summed up in the single sentence: Abolition of private property."

"For a long time past, slavery, serfdom, and every form of exploitation of one man by another, had already, in the western world, disappeared in theory and in practice."
"In modern bourgeois society, private property in the product of labour, and consequently private property in the labourer himself, is the foundation of the whole social system."
"If the proletariat during its contest with the bourgeoisie is compelled, by the force of circumstances, to organise itself as a class, if, in order to win, it is compelled to rise, as a class, to political supremacy, then the theory of the Communists is to abolish classes altogether."
"Conservatism, the middle-class ideology, natal for every newly developed class, — the bourgeoisie, has created its own future grave-diggers: its own proletariat."
"The condition for the existence, and for the sway of the bourgeois class, is the constantly reproducible, economical preponderance of the propertied conditions of production."
"In proportion as the bourgeoisie, i.e., capital, is developed, in the same proportion is the proletariat, the modern working class, developed — a class of labourers, who live only so long as they find work, and who find work only so long as their labour increases capital."
"These labourers, who must sell themselves piecemeal, are a commodity, like every other article of commerce."
"The proletarians have nothing to lose but their chains. They have a world to win."
"WORKING MEN OF ALL COUNTRIES, UNITE!");
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

int main()
{
    HAL_Init();
    SystemClock_Config();

    xTaskCreateStatic(Init_Task,
                    "Init Task",
                    configMINIMAL_STACK_SIZE,
                    NULL,
                    tskIDLE_PRIORITY + 1,
                    initTaskStack,
                    &initTaskBuffer
                );

    xTaskCreateStatic(Blink_Task,
                    "Blink Task",
                    configMINIMAL_STACK_SIZE,
                    NULL,
                    tskIDLE_PRIORITY + 1,
                    blinkTaskStack,
                    &blinkTaskBuffer
                );

    xTaskCreateStatic(ADC_ConvResult_Task,
                    "ADC Conversion Result Task",
                    configMINIMAL_STACK_SIZE,
                    NULL,
                    tskIDLE_PRIORITY + 1,
                    adcConvTaskStack,
                    &adcConvTaskBuffer
                );

    vTaskStartScheduler();

    while(1) {}
}

/**
  * @brief UART MSP Initialization
  * This function configures the hardware resources used in this example
  * @param huart: UART handle pointer
  * @retval None
  */
void HAL_UART_MspGPIOInit(UART_HandleTypeDef* huart)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
  if(huart->Instance==USART3)
  {
  /** Initializes the peripherals clocks
  */
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART3;
    PeriphClkInit.Usart3ClockSelection = RCC_USART3CLKSOURCE_PCLK1;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
      Error_Handler();
    }

    /* Peripheral clock enable */
    __HAL_RCC_USART3_CLK_ENABLE();

    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    /**USART3 GPIO Configuration
    PB10     ------> USART3_TX
    PC11     ------> USART3_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
  }

}
