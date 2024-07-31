#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/devicetree.h>


#define THREAD_COMMUNICATOR_STACK 512
#define THREAD_COMMUNICATOR_PRIORITY 5
#define THREAD_CALCULATE_STACK 512
#define THREAD_CALCULATE_PRIORITY 4
#define THREAD_PROCESS_INPUTS_STACK 256
#define THREAD_PROCESS_INPUTS_PRIORITY 3

#define GPIOC_NODE DT_NODELABEL(gpioc)


void communicator_entry(void*, void*, void*);
void calculate_entry(void *, void *, void *);
void process_inputs_entry(void *, void *, void *);

void adc_sampler_cb(struct k_timer *tim_id);
void button_press(const struct device *dev, struct gpio_callback *cb, gpio_port_pins_t pins);

struct gpio_callback button_cb_data;
const struct device *gpioc_dev = DEVICE_DT_GET(GPIOC_NODE);
static const struct adc_dt_spec adc_channel = ADC_DT_SPEC_GET(DT_PATH(zephyr_user));


K_MSGQ_DEFINE(adc_data, 30,15,1);

K_TIMER_DEFINE(adc_sampler,adc_sampler_cb,NULL);

// K_THREAD_STACK_DEFINE(communicator_area, THREAD_COMMUNICATOR_STACK);
// struct k_thread communicator_data;
// K_THREAD_STACK_DEFINE(calculate_area, THREAD_CALCULATE_STACK);
// k_tid_t communicator;
// K_THREAD_STACK_DEFINE(process_inputs_area, THREAD_PROCESS_INPUTS_STACK);
// k_tid_t communicator;

K_THREAD_DEFINE(communicator, THREAD_COMMUNICATOR_STACK,communicator_entry, NULL, NULL, NULL, THREAD_COMMUNICATOR_PRIORITY, 0, K_TICKS_FOREVER);
K_THREAD_DEFINE(calculate, THREAD_CALCULATE_STACK, calculate_entry, NULL, NULL, NULL, THREAD_CALCULATE_PRIORITY, 0, K_TICKS_FOREVER);
K_THREAD_DEFINE(process_input, THREAD_PROCESS_INPUTS_STACK, process_inputs_entry, NULL, NULL, NULL, THREAD_PROCESS_INPUTS_PRIORITY, 0, K_TICKS_FOREVER);

int16_t buf = 0;
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

    // k_queue_init(&adc_data);

    // k_tid_t communicator_id = k_thread_create(&communicator_data, communicator_area, K_THREAD_STACK_SIZEOF(communicator_area), communicator_entry,NULL, NULL, NULL, THREAD_COMMUNICATOR_PRIORITY,0, K_FOREVER);


    printk("init completed");
    

    return 0;
}

void communicator_entry(void* d1, void *d2, void *d3) {

    ARG_UNUSED(d1);
    ARG_UNUSED(d2);
    ARG_UNUSED(d3);
    

}

void calculate_entry(void* d1, void *d2, void *d3) {
    ARG_UNUSED(d1);
    ARG_UNUSED(d2);
    ARG_UNUSED(d3);


    volatile int16_t adc_val;

    for(;;) {
        if(k_msgq_num_used_get(&adc_data) > 0) {

            k_msgq_get(&adc_data,&adc_val,K_NO_WAIT);
            printk("%d ",adc_val);

        }else {
            //printk("dupa");

        }
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
            printk("ashkdakhskdhas");
            k_timer_start(&adc_sampler, K_MSEC(5), K_MSEC(20));


        } else {

            printk("timer_stopping!\n");
            pressedTimes = 0;

            k_timer_stop(&adc_sampler);

        }

        k_thread_suspend(process_input);
    }
        

        
    



}

//timer 
void adc_sampler_cb(struct k_timer *tim_id){
    
    adc_read(adc_channel.dev, &sequence);
    k_msgq_put(&adc_data,&buf,K_NO_WAIT);
    printk(".");

}

//button interrupt
void button_press(const struct device *dev, struct gpio_callback *cb, gpio_port_pins_t pins) {
	
    printk("dzialam");
    k_thread_start(process_input);
    
    k_thread_resume(process_input);
}