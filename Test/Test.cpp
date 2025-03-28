#include "pch.h"
#include "CppUnitTest.h"
#include "Engine/Registry/Registry.h"
#include <memory>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

struct A { int value = 42; };
struct B { float data = 3.14f; };
struct C { bool flag = true; };

namespace Test
{
    TEST_CLASS(RegistryTests)
    {
    public:
        TEST_METHOD(TestEntityCreation)
        {
            auto registry = std::make_shared<Registry<32>>();
            auto entity = registry->CreateEntity();

            Assert::AreEqual(0u, entity, L"First entity should be 0");
            Assert::AreEqual(1u, registry->GetCounter(), L"Counter should increment");
            Assert::IsTrue(registry->HasComponents<ComponentBitset<32>>(entity), L"Entity should have ComponentBitset");
            Assert::IsTrue(registry->HasComponents<Relationship>(entity), L"Entity should have Relationship");
            Assert::IsTrue(registry->GetDestroyedEntities().empty(), L"Destroyed entities should be empty");
        }
    };
}
