#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

MODULE_LICENSE( "GPL" );
MODULE_AUTHOR( "Liga" );
MODULE_DESCRIPTION( "Simple linux driver made to interact with processes." );
MODULE_VERSION( "1.0" );

static int __init simple_start( void )
{
	printk( KERN_INFO "[simple_diver] Driver has been loaded." );

	return 0;
}

static void __exit simple_end( void ) { }

module_init( simple_start );
module_exit( simple_end );