#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/devicetree.h>
#include <zephyr/sys/ring_buffer.h>


#define THREAD_COMMUNICATOR_STACK 512
#define THREAD_COMMUNICATOR_PRIORITY 5
#define THREAD_CALCULATE_STACK 512
#define THREAD_CALCULATE_PRIORITY 4
#define THREAD_PROCESS_INPUTS_STACK 256
#define THREAD_PROCESS_INPUTS_PRIORITY 3

#define GPIOC_NODE DT_NODELABEL(gpioc)

#define RING_BUF_BYTES 8

void communicator_entry(void*, void*, void*);
void calculate_entry(void *, void *, void *);
void process_inputs_entry(void *, void *, void *);

void adc_sampler_cb(struct k_timer *tim_id);
void heart_beat_timer_cb(struct k_timer *tim_id);
void button_press(const struct device *dev, struct gpio_callback *cb, gpio_port_pins_t pins);

struct gpio_callback button_cb_data;
const struct device *gpioc_dev = DEVICE_DT_GET(GPIOC_NODE);
static const struct adc_dt_spec adc_channel = ADC_DT_SPEC_GET(DT_PATH(zephyr_user));


//RING_BUF_DECLARE(circ_buffer, RING_BUF_BYTES);

K_MSGQ_DEFINE(adc_data, 2,15,1);

K_TIMER_DEFINE(adc_sampler,adc_sampler_cb,NULL);
K_TIMER_DEFINE(heart_beat_timer,heart_beat_timer_cb,NULL);


K_THREAD_DEFINE(communicator, THREAD_COMMUNICATOR_STACK,communicator_entry, NULL, NULL, NULL, THREAD_COMMUNICATOR_PRIORITY, 0, K_TICKS_FOREVER);
K_THREAD_DEFINE(calculate, THREAD_CALCULATE_STACK, calculate_entry, NULL, NULL, NULL, THREAD_CALCULATE_PRIORITY, 0, K_TICKS_FOREVER);
K_THREAD_DEFINE(process_input, THREAD_PROCESS_INPUTS_STACK, process_inputs_entry, NULL, NULL, NULL, THREAD_PROCESS_INPUTS_PRIORITY, 0, K_TICKS_FOREVER);

uint16_t buf = 0;
struct adc_sequence sequence = {
    .buffer = &buf,
    .buffer_size = sizeof(buf),

};

int main() {

    int ret=0;

    ret = gpio_pin_configure(gpioc_dev, 13, GPIO_INPUT);
    if (ret < 0) {
        printk("gpio_pin_config error");
    }

    ret = gpio_pin_interrupt_configure(gpioc_dev, 13, GPIO_INT_EDGE_RISING);
    if (ret < 0) {
        printk("gpio_pin_interrrupt_config error");
    }

    gpio_init_callback(&button_cb_data, button_press,BIT(13));

    ret = gpio_add_callback(gpioc_dev, &button_cb_data);
    if (ret < 0) {
        printk("adc_add_callback error");
    }

    ret = adc_channel_setup_dt(&adc_channel);
    if (ret < 0) {
        printk("adc_channel_setup error");
    }

    ret = adc_sequence_init_dt(&adc_channel,&sequence);
    if (ret < 0) {
        printk("adc_sequence_init error");
    }


    printk("init completed");
    

    return 0;
}



void communicator_entry(void* d1, void *d2, void *d3) {

    ARG_UNUSED(d1);
    ARG_UNUSED(d2);
    ARG_UNUSED(d3);
    

}

uint8_t heart_flag = 0;

void calculate_entry(void* d1, void *d2, void *d3) {
    ARG_UNUSED(d1);
    ARG_UNUSED(d2);
    ARG_UNUSED(d3);

    uint16_t circ_buffer[128] = {0};
    uint16_t adc_val;
    uint8_t counter = 0;
    uint8_t fallingEdge = 0;
    uint8_t heartBeat = 0;

    for(;;) {

        if(k_msgq_num_used_get(&adc_data) > 0) {

            k_msgq_get(&adc_data,&adc_val,K_NO_WAIT);
            circ_buffer[counter++] = adc_val;


            // for(int i = 0; i < 128; i++) {
            //     printk("%d ",circ_buffer[i]);
            // }
            // printk("\n");

            for(int i = 0; i < 50; i++) {

                if( (adc_val + 10) < circ_buffer[(counter - i) & 127]) {
                    fallingEdge++;
                }

            }

            if(fallingEdge > 40 && heart_flag == 0) {
                //a heart beat occured
                printk("puk \n");

                k_timer_start(&heart_beat_timer,K_MSEC(300),K_MSEC(300));
                heart_flag = 1;
            }
            // printk("%d", heart_flag);
        }else {
            //printk("dupa");
            
        }

        fallingEdge = 0;

        counter &= 127;

        k_msleep(10);
    }
    
    //k_thread_suspend(calculate);
    

    
}



void process_inputs_entry(void* d1, void *d2, void *d3){
    ARG_UNUSED(d1);
    ARG_UNUSED(d2);
    ARG_UNUSED(d3);

    static uint8_t pressedTimes = 0;


    for(;;) {

        if(pressedTimes == 0) {

            printk("timer_starting!\n");
            pressedTimes= 1;
            k_thread_start(calculate);
            k_thread_resume(calculate);
            k_timer_start(&adc_sampler, K_MSEC(0), K_MSEC(20));


        } else {

            printk("timer_stopping!\n");
            pressedTimes = 0;

            k_timer_stop(&adc_sampler);
            k_thread_suspend(calculate);

        }

        k_thread_suspend(process_input);
    }
        

}

//timer 
void adc_sampler_cb(struct k_timer *tim_id){
    
    adc_read(adc_channel.dev, &sequence);
    k_msgq_put(&adc_data,(uint16_t*)&buf,K_NO_WAIT);
    // ring_buf_put(&circ_buffer,(uint8_t*)&buf,2);

}

void heart_beat_timer_cb(struct k_timer *tim_id){

    heart_flag = 0;
    // printk("jestem");
}

//button interrupt
void button_press(const struct device *dev, struct gpio_callback *cb, gpio_port_pins_t pins) {
	
    k_thread_start(process_input);
    
    k_thread_resume(process_input);
}