#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/device.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/io.h>
#include <linux/errno.h>

const char *reg_control_name = "control";
const char *reg_data_name = "data";
const char *tx_interrupt_name = "tx_irq";
const char *rx_interrupt_name = "rx_irq";

struct test_dtb_data
{
    void __iomem *base_control;
    void __iomem *base_data;
    int irq_rx;
    int irq_tx;
    int buffer_size;
    const char *dvc_name;
};

static int test_probe(struct platform_device *pDev)
{
    int ret = 0;
    struct resource *res_control = NULL;
    struct resource *res_data    = NULL;
    struct test_dtb_data *dtc_data = NULL;

    pr_info("TEST: Probe called\n");
    dtc_data = (struct test_dtb_data*)devm_kzalloc(&pDev->dev, sizeof(struct test_dtb_data), GFP_KERNEL);
    if(dtc_data == NULL)
    {
        pr_err("devm_kzalloc Failed \n");
        return -ENOMEM;
    }
    
    res_control = platform_get_resource_byname(pDev, IORESOURCE_MEM, reg_control_name);
    if(!(res_control))
    {
        pr_err("platform_get_resource_byname control \n");
        return -EINVAL;
    }
    pr_info("start=%pa end=%pa\n", &res_control->start, &res_control->end);

    dtc_data->base_control = devm_ioremap_resource(&pDev->dev, res_control);
    if(!(dtc_data->base_control))
    {
        pr_err("devm_ioremap base_control \n");
        return -ENODEV;
    }

    res_data = platform_get_resource_byname(pDev, IORESOURCE_MEM, reg_data_name);
    if(!(res_data))
    {
        pr_err("platform_get_resource_byname data \n");
        return -EINVAL;
    }
    pr_info("start=%pa end=%pa\n", &res_data->start, &res_data->end);

    dtc_data->base_data = devm_ioremap_resource(&pDev->dev, res_data);
    if(!(dtc_data->base_data))
    {
        pr_err("devm_ioremap base_data \n");
        return -ENODEV;
    }

    dtc_data->irq_rx = platform_get_irq_byname(pDev, rx_interrupt_name);
    if(ret!= 0)
    {
        pr_err("platform_get_irq_byname rx_irq \n");
        return ret;
    }
    pr_info("irq_name_rx =>[%d]\n", dtc_data->irq_rx);

    dtc_data->irq_tx = platform_get_irq_byname(pDev, tx_interrupt_name);
    if(ret!= 0)
    {
        pr_err("platform_get_irq_byname tx_irq \n");
        return ret;
    }
    pr_info("irq_name_tx =>[%d]\n", dtc_data->irq_tx);

    ret = of_property_read_string(pDev->dev.of_node, "device-name", &dtc_data->dvc_name);
    if(ret!= 0)
    {
        pr_err("of_property_read_string string \n");
        return ret;
    }

    pr_info("device - name=>[%s]\n", dtc_data->dvc_name);

    ret = of_property_read_u32(pDev->dev.of_node, "buffer-size", &dtc_data->buffer_size);
    if(ret!= 0)
    {
        pr_err("of_property_read_string int \n");
        return ret;
    }

    pr_info("device - buffer-size=>[%d]\n", dtc_data->buffer_size);

    pr_info("TEST: Probe Successfull\n");
    return 0;
}

static void test_remove(struct platform_device *pDev)
{
    pr_info("TEST: Remove called\n");
}

static const struct of_device_id test_of_match[] = {
    { .compatible = "raj,test-device" },
    { }
};

MODULE_DEVICE_TABLE(of, test_of_match);

static struct platform_driver test_driver = {
    .probe  = test_probe,
    .remove = test_remove,
    .driver = {
        .name = "test_driver",
        .of_match_table = test_of_match,
    },
};

module_platform_driver(test_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Raj Kumar");
MODULE_DESCRIPTION(" ARCH64 platform Module");