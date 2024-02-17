#include <iostream>
#include "GraphicsApp.h"

int main()
{
	try
	{
		Anni::GraphicsApp app_instance{};
		app_instance.Run();
	}
	catch (vk::SystemError& err)
	{
		std::cout << "vk::SystemError: " << err.what() << std::endl;
		exit(-1);
	}
	catch (std::exception& err)
	{
		std::cout << "std::exception: " << err.what() << std::endl;
		exit(-1);
	}
	catch (...)
	{
		std::cout << "unknown error\n";
		exit(-1);
	}
	return EXIT_SUCCESS;
}

