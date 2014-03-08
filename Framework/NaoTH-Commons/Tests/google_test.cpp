#undef DEBUG

#include <glib.h>
#include <glib-object.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

void trash_log_handler (const gchar* /*log_domain*/, GLogLevelFlags /*log_level*/, const gchar* /*message*/, gpointer /*userdata*/)
{
  // do nothing...
}

int main(int argc, char *argv[])
{
  g_type_init();
  g_log_set_default_handler(&trash_log_handler, NULL);

  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}
