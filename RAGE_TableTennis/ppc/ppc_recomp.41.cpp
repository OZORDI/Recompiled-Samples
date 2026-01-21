#include "ppc_recomp_shared.h"

PPC_FUNC_IMPL(__imp__sub_824F4938) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f860
	ctx.lr = 0x824F4940;
	sub_8242F860(ctx, base);
	// stwu r1,-320(r1)
	ea = -320 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_824F4938) {
	__imp__sub_824F4938(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_824F4CF8) {
	PPC_FUNC_PROLOGUE();
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f894
	ctx.lr = 0x824F4D00;
	__savegprlr_27(ctx, base);
}

PPC_WEAK_FUNC(sub_824F4CF8) {
	__imp__sub_824F4CF8(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_824F4D90) {
	PPC_FUNC_PROLOGUE();
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f894
	ctx.lr = 0x824F4D98;
	__savegprlr_27(ctx, base);
}

PPC_WEAK_FUNC(sub_824F4D90) {
	__imp__sub_824F4D90(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_824F4FF0) {
	PPC_FUNC_PROLOGUE();
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f894
	ctx.lr = 0x824F4FF8;
	__savegprlr_27(ctx, base);
}

PPC_WEAK_FUNC(sub_824F4FF0) {
	__imp__sub_824F4FF0(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_824F5140) {
	PPC_FUNC_PROLOGUE();
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f874
	ctx.lr = 0x824F5148;
	__savegprlr_19(ctx, base);
}

PPC_WEAK_FUNC(sub_824F5140) {
	__imp__sub_824F5140(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_824F5240) {
	PPC_FUNC_PROLOGUE();
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f86c
	ctx.lr = 0x824F5248;
	__savegprlr_17(ctx, base);
}

PPC_WEAK_FUNC(sub_824F5240) {
	__imp__sub_824F5240(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_824F5438) {
	PPC_FUNC_PROLOGUE();
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f860
	ctx.lr = 0x824F5440;
	sub_8242F860(ctx, base);
}

PPC_WEAK_FUNC(sub_824F5438) {
	__imp__sub_824F5438(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_824F55F0) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f870
	ctx.lr = 0x824F55F8;
	__savegprlr_18(ctx, base);
	// stwu r1,-224(r1)
	ea = -224 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_824F55F0) {
	__imp__sub_824F55F0(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_824F5750) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f870
	ctx.lr = 0x824F5758;
	__savegprlr_18(ctx, base);
	// stwu r1,-224(r1)
	ea = -224 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_824F5750) {
	__imp__sub_824F5750(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_824F58A0) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f860
	ctx.lr = 0x824F58A8;
	sub_8242F860(ctx, base);
	// stwu r1,-288(r1)
	ea = -288 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_824F58A0) {
	__imp__sub_824F58A0(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_824F5AC0) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f874
	ctx.lr = 0x824F5AC8;
	__savegprlr_19(ctx, base);
	// stwu r1,-224(r1)
	ea = -224 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_824F5AC0) {
	__imp__sub_824F5AC0(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_824F5BC8) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f87c
	ctx.lr = 0x824F5BD0;
	__savegprlr_21(ctx, base);
	// stwu r1,-176(r1)
	ea = -176 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_824F5BC8) {
	__imp__sub_824F5BC8(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_824F5C78) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f880
	ctx.lr = 0x824F5C80;
	__savegprlr_22(ctx, base);
	// stwu r1,-176(r1)
	ea = -176 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_824F5C78) {
	__imp__sub_824F5C78(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_824F5D30) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f878
	ctx.lr = 0x824F5D38;
	__savegprlr_20(ctx, base);
	// stwu r1,-192(r1)
	ea = -192 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_824F5D30) {
	__imp__sub_824F5D30(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_824F5FA8) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f894
	ctx.lr = 0x824F5FB0;
	__savegprlr_27(ctx, base);
	// stwu r1,-640(r1)
	ea = -640 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_824F5FA8) {
	__imp__sub_824F5FA8(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_824F6030) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f894
	ctx.lr = 0x824F6038;
	__savegprlr_27(ctx, base);
	// stwu r1,-640(r1)
	ea = -640 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_824F6030) {
	__imp__sub_824F6030(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_824F60B8) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// stw r12,-8(r1)
	PPC_STORE_U32(ctx.r1.u32 + -8, ctx.r12.u32);
	// std r30,-24(r1)
	PPC_STORE_U64(ctx.r1.u32 + -24, ctx.r30.u64);
	// std r31,-16(r1)
	PPC_STORE_U64(ctx.r1.u32 + -16, ctx.r31.u64);
	// stwu r1,-112(r1)
	ea = -112 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_824F60B8) {
	__imp__sub_824F60B8(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_824F6518) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f894
	ctx.lr = 0x824F6520;
	__savegprlr_27(ctx, base);
	// stwu r1,-128(r1)
	ea = -128 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_824F6518) {
	__imp__sub_824F6518(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_824F66C0) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f88c
	ctx.lr = 0x824F66C8;
	__savegprlr_25(ctx, base);
	// stwu r1,-144(r1)
	ea = -144 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_824F66C0) {
	__imp__sub_824F66C0(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_824F6E20) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f890
	ctx.lr = 0x824F6E28;
	__savegprlr_26(ctx, base);
	// stwu r1,-144(r1)
	ea = -144 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_824F6E20) {
	__imp__sub_824F6E20(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_824F71D8) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f898
	ctx.lr = 0x824F71E0;
	__savegprlr_28(ctx, base);
	// stwu r1,-128(r1)
	ea = -128 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_824F71D8) {
	__imp__sub_824F71D8(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_824F76D8) {
	PPC_FUNC_PROLOGUE();
	// std r31,-8(r1)
	PPC_STORE_U64(ctx.r1.u32 + -8, ctx.r31.u64);
}

PPC_WEAK_FUNC(sub_824F76D8) {
	__imp__sub_824F76D8(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_824F7820) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f888
	ctx.lr = 0x824F7828;
	__savegprlr_24(ctx, base);
	// stwu r1,-160(r1)
	ea = -160 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_824F7820) {
	__imp__sub_824F7820(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_824F7CA0) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f89c
	ctx.lr = 0x824F7CA8;
	__savegprlr_29(ctx, base);
	// stwu r1,-128(r1)
	ea = -128 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_824F7CA0) {
	__imp__sub_824F7CA0(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_824F7D90) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f894
	ctx.lr = 0x824F7D98;
	__savegprlr_27(ctx, base);
	// stwu r1,-128(r1)
	ea = -128 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_824F7D90) {
	__imp__sub_824F7D90(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_824F7F20) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f860
	ctx.lr = 0x824F7F28;
	sub_8242F860(ctx, base);
	// stwu r1,-256(r1)
	ea = -256 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_824F7F20) {
	__imp__sub_824F7F20(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_824F9230) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f884
	ctx.lr = 0x824F9238;
	__savegprlr_23(ctx, base);
	// stwu r1,-160(r1)
	ea = -160 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_824F9230) {
	__imp__sub_824F9230(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_824F9808) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f860
	ctx.lr = 0x824F9810;
	sub_8242F860(ctx, base);
	// stwu r1,-240(r1)
	ea = -240 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_824F9808) {
	__imp__sub_824F9808(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_824F99B0) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f860
	ctx.lr = 0x824F99B8;
	sub_8242F860(ctx, base);
	// stwu r1,-240(r1)
	ea = -240 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_824F99B0) {
	__imp__sub_824F99B0(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_824F9B68) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f860
	ctx.lr = 0x824F9B70;
	sub_8242F860(ctx, base);
	// stwu r1,-256(r1)
	ea = -256 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_824F9B68) {
	__imp__sub_824F9B68(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_824F9EE8) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f880
	ctx.lr = 0x824F9EF0;
	__savegprlr_22(ctx, base);
	// stwu r1,-176(r1)
	ea = -176 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_824F9EE8) {
	__imp__sub_824F9EE8(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_824FA100) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f884
	ctx.lr = 0x824FA108;
	__savegprlr_23(ctx, base);
	// stwu r1,-160(r1)
	ea = -160 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_824FA100) {
	__imp__sub_824FA100(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_824FA248) {
	PPC_FUNC_PROLOGUE();
	// std r31,-8(r1)
	PPC_STORE_U64(ctx.r1.u32 + -8, ctx.r31.u64);
}

PPC_WEAK_FUNC(sub_824FA248) {
	__imp__sub_824FA248(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_824FA370) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f860
	ctx.lr = 0x824FA378;
	sub_8242F860(ctx, base);
	// stwu r1,-240(r1)
	ea = -240 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_824FA370) {
	__imp__sub_824FA370(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_824FA548) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f860
	ctx.lr = 0x824FA550;
	sub_8242F860(ctx, base);
	// stwu r1,-240(r1)
	ea = -240 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_824FA548) {
	__imp__sub_824FA548(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_824FA710) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f898
	ctx.lr = 0x824FA718;
	__savegprlr_28(ctx, base);
	// stwu r1,-128(r1)
	ea = -128 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_824FA710) {
	__imp__sub_824FA710(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_824FAA20) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f898
	ctx.lr = 0x824FAA28;
	__savegprlr_28(ctx, base);
	// stwu r1,-128(r1)
	ea = -128 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_824FAA20) {
	__imp__sub_824FAA20(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_824FAD20) {
	PPC_FUNC_PROLOGUE();
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f874
	ctx.lr = 0x824FAD28;
	__savegprlr_19(ctx, base);
}

PPC_WEAK_FUNC(sub_824FAD20) {
	__imp__sub_824FAD20(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_824FB5C8) {
	PPC_FUNC_PROLOGUE();
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f874
	ctx.lr = 0x824FB5D0;
	__savegprlr_19(ctx, base);
}

PPC_WEAK_FUNC(sub_824FB5C8) {
	__imp__sub_824FB5C8(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_824FBE70) {
	PPC_FUNC_PROLOGUE();
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f874
	ctx.lr = 0x824FBE78;
	__savegprlr_19(ctx, base);
}

PPC_WEAK_FUNC(sub_824FBE70) {
	__imp__sub_824FBE70(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_824FC050) {
	PPC_FUNC_PROLOGUE();
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f860
	ctx.lr = 0x824FC058;
	sub_8242F860(ctx, base);
}

PPC_WEAK_FUNC(sub_824FC050) {
	__imp__sub_824FC050(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_824FC348) {
	PPC_FUNC_PROLOGUE();
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f860
	ctx.lr = 0x824FC350;
	sub_8242F860(ctx, base);
}

PPC_WEAK_FUNC(sub_824FC348) {
	__imp__sub_824FC348(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_824FCCE0) {
	PPC_FUNC_PROLOGUE();
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f860
	ctx.lr = 0x824FCCE8;
	sub_8242F860(ctx, base);
}

PPC_WEAK_FUNC(sub_824FCCE0) {
	__imp__sub_824FCCE0(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_824FD678) {
	PPC_FUNC_PROLOGUE();
	// std r30,-16(r1)
	PPC_STORE_U64(ctx.r1.u32 + -16, ctx.r30.u64);
	// std r31,-8(r1)
	PPC_STORE_U64(ctx.r1.u32 + -8, ctx.r31.u64);
}

PPC_WEAK_FUNC(sub_824FD678) {
	__imp__sub_824FD678(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_824FD900) {
	PPC_FUNC_PROLOGUE();
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f87c
	ctx.lr = 0x824FD908;
	__savegprlr_21(ctx, base);
}

PPC_WEAK_FUNC(sub_824FD900) {
	__imp__sub_824FD900(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_824FDA80) {
	PPC_FUNC_PROLOGUE();
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f860
	ctx.lr = 0x824FDA88;
	sub_8242F860(ctx, base);
}

PPC_WEAK_FUNC(sub_824FDA80) {
	__imp__sub_824FDA80(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_824FDCD8) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f88c
	ctx.lr = 0x824FDCE0;
	__savegprlr_25(ctx, base);
	// stwu r1,-144(r1)
	ea = -144 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_824FDCD8) {
	__imp__sub_824FDCD8(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_824FDF10) {
	PPC_FUNC_PROLOGUE();
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f868
	ctx.lr = 0x824FDF18;
	__savegprlr_16(ctx, base);
}

PPC_WEAK_FUNC(sub_824FDF10) {
	__imp__sub_824FDF10(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_824FE018) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f898
	ctx.lr = 0x824FE020;
	__savegprlr_28(ctx, base);
	// stwu r1,-144(r1)
	ea = -144 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_824FE018) {
	__imp__sub_824FE018(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_824FE258) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f888
	ctx.lr = 0x824FE260;
	__savegprlr_24(ctx, base);
	// stwu r1,-160(r1)
	ea = -160 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_824FE258) {
	__imp__sub_824FE258(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_824FE380) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f874
	ctx.lr = 0x824FE388;
	__savegprlr_19(ctx, base);
	// stwu r1,-192(r1)
	ea = -192 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_824FE380) {
	__imp__sub_824FE380(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_824FE780) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f878
	ctx.lr = 0x824FE788;
	__savegprlr_20(ctx, base);
	// stwu r1,-192(r1)
	ea = -192 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_824FE780) {
	__imp__sub_824FE780(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_824FE980) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f874
	ctx.lr = 0x824FE988;
	__savegprlr_19(ctx, base);
	// stwu r1,-192(r1)
	ea = -192 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_824FE980) {
	__imp__sub_824FE980(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_824FEB68) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f860
	ctx.lr = 0x824FEB70;
	sub_8242F860(ctx, base);
	// stwu r1,-288(r1)
	ea = -288 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_824FEB68) {
	__imp__sub_824FEB68(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_824FF078) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f888
	ctx.lr = 0x824FF080;
	__savegprlr_24(ctx, base);
	// stwu r1,-160(r1)
	ea = -160 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_824FF078) {
	__imp__sub_824FF078(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_824FF1D8) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f860
	ctx.lr = 0x824FF1E0;
	sub_8242F860(ctx, base);
	// stwu r1,-240(r1)
	ea = -240 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_824FF1D8) {
	__imp__sub_824FF1D8(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_824FF628) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f878
	ctx.lr = 0x824FF630;
	__savegprlr_20(ctx, base);
	// stwu r1,-224(r1)
	ea = -224 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_824FF628) {
	__imp__sub_824FF628(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_824FF7D0) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f860
	ctx.lr = 0x824FF7D8;
	sub_8242F860(ctx, base);
	// stfd f29,-176(r1)
	ctx.fpscr.disableFlushMode();
	PPC_STORE_U64(ctx.r1.u32 + -176, ctx.f29.u64);
	// stfd f30,-168(r1)
	PPC_STORE_U64(ctx.r1.u32 + -168, ctx.f30.u64);
	// stfd f31,-160(r1)
	PPC_STORE_U64(ctx.r1.u32 + -160, ctx.f31.u64);
	// stwu r1,-352(r1)
	ea = -352 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_824FF7D0) {
	__imp__sub_824FF7D0(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82500568) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f860
	ctx.lr = 0x82500570;
	sub_8242F860(ctx, base);
	// addi r12,r1,-152
	ctx.r12.s64 = ctx.r1.s64 + -152;
	// bl 0x82436618
	ctx.lr = 0x82500578;
	__savefpr_28(ctx, base);
	// stwu r1,-352(r1)
	ea = -352 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_82500568) {
	__imp__sub_82500568(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82501D08) {
	PPC_FUNC_PROLOGUE();
	// std r31,-8(r1)
	PPC_STORE_U64(ctx.r1.u32 + -8, ctx.r31.u64);
}

PPC_WEAK_FUNC(sub_82501D08) {
	__imp__sub_82501D08(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82502130) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f860
	ctx.lr = 0x82502138;
	sub_8242F860(ctx, base);
	// addi r12,r1,-152
	ctx.r12.s64 = ctx.r1.s64 + -152;
	// bl 0x82436604
	ctx.lr = 0x82502140;
	__savefpr_23(ctx, base);
	// stwu r1,-464(r1)
	ea = -464 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_82502130) {
	__imp__sub_82502130(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_825035F0) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f860
	ctx.lr = 0x825035F8;
	sub_8242F860(ctx, base);
	// addi r12,r1,-152
	ctx.r12.s64 = ctx.r1.s64 + -152;
	// bl 0x82436600
	ctx.lr = 0x82503600;
	__savefpr_22(ctx, base);
	// stwu r1,-464(r1)
	ea = -464 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_825035F0) {
	__imp__sub_825035F0(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82504A68) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f860
	ctx.lr = 0x82504A70;
	sub_8242F860(ctx, base);
	// addi r12,r1,-152
	ctx.r12.s64 = ctx.r1.s64 + -152;
	// bl 0x82436608
	ctx.lr = 0x82504A78;
	__savefpr_24(ctx, base);
	// stwu r1,-448(r1)
	ea = -448 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_82504A68) {
	__imp__sub_82504A68(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82505E80) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f860
	ctx.lr = 0x82505E88;
	sub_8242F860(ctx, base);
	// addi r12,r1,-152
	ctx.r12.s64 = ctx.r1.s64 + -152;
	// bl 0x82436604
	ctx.lr = 0x82505E90;
	__savefpr_23(ctx, base);
	// stwu r1,-448(r1)
	ea = -448 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_82505E80) {
	__imp__sub_82505E80(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82507258) {
	PPC_FUNC_PROLOGUE();
	// std r31,-8(r1)
	PPC_STORE_U64(ctx.r1.u32 + -8, ctx.r31.u64);
}

PPC_WEAK_FUNC(sub_82507258) {
	__imp__sub_82507258(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82507630) {
	PPC_FUNC_PROLOGUE();
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f888
	ctx.lr = 0x82507638;
	__savegprlr_24(ctx, base);
}

PPC_WEAK_FUNC(sub_82507630) {
	__imp__sub_82507630(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82507798) {
	PPC_FUNC_PROLOGUE();
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f884
	ctx.lr = 0x825077A0;
	__savegprlr_23(ctx, base);
}

PPC_WEAK_FUNC(sub_82507798) {
	__imp__sub_82507798(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82507BD0) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f860
	ctx.lr = 0x82507BD8;
	sub_8242F860(ctx, base);
	// addi r12,r1,-152
	ctx.r12.s64 = ctx.r1.s64 + -152;
	// bl 0x82436608
	ctx.lr = 0x82507BE0;
	__savefpr_24(ctx, base);
	// stwu r1,-336(r1)
	ea = -336 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_82507BD0) {
	__imp__sub_82507BD0(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82508098) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// stw r12,-8(r1)
	PPC_STORE_U32(ctx.r1.u32 + -8, ctx.r12.u32);
	// stwu r1,-96(r1)
	ea = -96 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_82508098) {
	__imp__sub_82508098(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82508238) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f87c
	ctx.lr = 0x82508240;
	__savegprlr_21(ctx, base);
	// stfd f30,-112(r1)
	ctx.fpscr.disableFlushMode();
	PPC_STORE_U64(ctx.r1.u32 + -112, ctx.f30.u64);
	// stfd f31,-104(r1)
	PPC_STORE_U64(ctx.r1.u32 + -104, ctx.f31.u64);
	// stwu r1,-208(r1)
	ea = -208 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_82508238) {
	__imp__sub_82508238(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_825089E8) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// stw r12,-8(r1)
	PPC_STORE_U32(ctx.r1.u32 + -8, ctx.r12.u32);
	// std r30,-24(r1)
	PPC_STORE_U64(ctx.r1.u32 + -24, ctx.r30.u64);
	// std r31,-16(r1)
	PPC_STORE_U64(ctx.r1.u32 + -16, ctx.r31.u64);
	// stwu r1,-112(r1)
	ea = -112 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_825089E8) {
	__imp__sub_825089E8(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82508AB8) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f894
	ctx.lr = 0x82508AC0;
	__savegprlr_27(ctx, base);
	// stwu r1,-128(r1)
	ea = -128 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_82508AB8) {
	__imp__sub_82508AB8(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82508D50) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f864
	ctx.lr = 0x82508D58;
	__savegprlr_15(ctx, base);
	// stwu r1,-224(r1)
	ea = -224 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_82508D50) {
	__imp__sub_82508D50(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_825091C8) {
	PPC_FUNC_PROLOGUE();
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f894
	ctx.lr = 0x825091D0;
	__savegprlr_27(ctx, base);
}

PPC_WEAK_FUNC(sub_825091C8) {
	__imp__sub_825091C8(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82509768) {
	PPC_FUNC_PROLOGUE();
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f88c
	ctx.lr = 0x82509770;
	__savegprlr_25(ctx, base);
}

PPC_WEAK_FUNC(sub_82509768) {
	__imp__sub_82509768(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82509C38) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f878
	ctx.lr = 0x82509C40;
	__savegprlr_20(ctx, base);
	// stfd f30,-120(r1)
	ctx.fpscr.disableFlushMode();
	PPC_STORE_U64(ctx.r1.u32 + -120, ctx.f30.u64);
	// stfd f31,-112(r1)
	PPC_STORE_U64(ctx.r1.u32 + -112, ctx.f31.u64);
	// stwu r1,-224(r1)
	ea = -224 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_82509C38) {
	__imp__sub_82509C38(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82509FC0) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f884
	ctx.lr = 0x82509FC8;
	__savegprlr_23(ctx, base);
	// stwu r1,-192(r1)
	ea = -192 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_82509FC0) {
	__imp__sub_82509FC0(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_8250A1D0) {
	PPC_FUNC_PROLOGUE();
	// std r30,-16(r1)
	PPC_STORE_U64(ctx.r1.u32 + -16, ctx.r30.u64);
	// std r31,-8(r1)
	PPC_STORE_U64(ctx.r1.u32 + -8, ctx.r31.u64);
}

PPC_WEAK_FUNC(sub_8250A1D0) {
	__imp__sub_8250A1D0(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_8250A808) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f898
	ctx.lr = 0x8250A810;
	__savegprlr_28(ctx, base);
	// addi r12,r1,-40
	ctx.r12.s64 = ctx.r1.s64 + -40;
	// bl 0x82436610
	ctx.lr = 0x8250A818;
	__savefpr_26(ctx, base);
	// stwu r1,-192(r1)
	ea = -192 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_8250A808) {
	__imp__sub_8250A808(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_8250ABF0) {
	PPC_FUNC_PROLOGUE();
	// std r30,-16(r1)
	PPC_STORE_U64(ctx.r1.u32 + -16, ctx.r30.u64);
	// std r31,-8(r1)
	PPC_STORE_U64(ctx.r1.u32 + -8, ctx.r31.u64);
}

PPC_WEAK_FUNC(sub_8250ABF0) {
	__imp__sub_8250ABF0(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_8250B9E0) {
	PPC_FUNC_PROLOGUE();
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f89c
	ctx.lr = 0x8250B9E8;
	__savegprlr_29(ctx, base);
	// stfd f29,-56(r1)
	ctx.fpscr.disableFlushMode();
	PPC_STORE_U64(ctx.r1.u32 + -56, ctx.f29.u64);
	// stfd f30,-48(r1)
	PPC_STORE_U64(ctx.r1.u32 + -48, ctx.f30.u64);
	// stfd f31,-40(r1)
	PPC_STORE_U64(ctx.r1.u32 + -40, ctx.f31.u64);
}

PPC_WEAK_FUNC(sub_8250B9E0) {
	__imp__sub_8250B9E0(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_8250C4D0) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f89c
	ctx.lr = 0x8250C4D8;
	__savegprlr_29(ctx, base);
	// stfd f29,-56(r1)
	ctx.fpscr.disableFlushMode();
	PPC_STORE_U64(ctx.r1.u32 + -56, ctx.f29.u64);
	// stfd f30,-48(r1)
	PPC_STORE_U64(ctx.r1.u32 + -48, ctx.f30.u64);
	// stfd f31,-40(r1)
	PPC_STORE_U64(ctx.r1.u32 + -40, ctx.f31.u64);
	// stwu r1,-176(r1)
	ea = -176 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_8250C4D0) {
	__imp__sub_8250C4D0(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_8250D158) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f870
	ctx.lr = 0x8250D160;
	__savegprlr_18(ctx, base);
	// stwu r1,-224(r1)
	ea = -224 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_8250D158) {
	__imp__sub_8250D158(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_8250D798) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// stw r12,-8(r1)
	PPC_STORE_U32(ctx.r1.u32 + -8, ctx.r12.u32);
	// std r30,-24(r1)
	PPC_STORE_U64(ctx.r1.u32 + -24, ctx.r30.u64);
	// std r31,-16(r1)
	PPC_STORE_U64(ctx.r1.u32 + -16, ctx.r31.u64);
	// stwu r1,-112(r1)
	ea = -112 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_8250D798) {
	__imp__sub_8250D798(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_8250D860) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f888
	ctx.lr = 0x8250D868;
	__savegprlr_24(ctx, base);
	// addi r12,r1,-72
	ctx.r12.s64 = ctx.r1.s64 + -72;
	// bl 0x82436614
	ctx.lr = 0x8250D870;
	__savefpr_27(ctx, base);
	// stwu r1,-336(r1)
	ea = -336 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_8250D860) {
	__imp__sub_8250D860(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_8250E2A8) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f898
	ctx.lr = 0x8250E2B0;
	__savegprlr_28(ctx, base);
	// stwu r1,-128(r1)
	ea = -128 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_8250E2A8) {
	__imp__sub_8250E2A8(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_8250E5D8) {
	PPC_FUNC_PROLOGUE();
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f890
	ctx.lr = 0x8250E5E0;
	__savegprlr_26(ctx, base);
}

PPC_WEAK_FUNC(sub_8250E5D8) {
	__imp__sub_8250E5D8(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_8250E6A8) {
	PPC_FUNC_PROLOGUE();
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f89c
	ctx.lr = 0x8250E6B0;
	__savegprlr_29(ctx, base);
}

PPC_WEAK_FUNC(sub_8250E6A8) {
	__imp__sub_8250E6A8(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_8250E7C8) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f888
	ctx.lr = 0x8250E7D0;
	__savegprlr_24(ctx, base);
	// stwu r1,-160(r1)
	ea = -160 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_8250E7C8) {
	__imp__sub_8250E7C8(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_8250E980) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f860
	ctx.lr = 0x8250E988;
	sub_8242F860(ctx, base);
	// stwu r1,-240(r1)
	ea = -240 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_8250E980) {
	__imp__sub_8250E980(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_8250ED38) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f860
	ctx.lr = 0x8250ED40;
	sub_8242F860(ctx, base);
	// stwu r1,-256(r1)
	ea = -256 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_8250ED38) {
	__imp__sub_8250ED38(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_8250F538) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f860
	ctx.lr = 0x8250F540;
	sub_8242F860(ctx, base);
	// stwu r1,-320(r1)
	ea = -320 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_8250F538) {
	__imp__sub_8250F538(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_8250FA00) {
	PPC_FUNC_PROLOGUE();
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f890
	ctx.lr = 0x8250FA08;
	__savegprlr_26(ctx, base);
}

PPC_WEAK_FUNC(sub_8250FA00) {
	__imp__sub_8250FA00(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_8250FB80) {
	PPC_FUNC_PROLOGUE();
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f860
	ctx.lr = 0x8250FB88;
	sub_8242F860(ctx, base);
}

PPC_WEAK_FUNC(sub_8250FB80) {
	__imp__sub_8250FB80(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82510120) {
	PPC_FUNC_PROLOGUE();
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f860
	ctx.lr = 0x82510128;
	sub_8242F860(ctx, base);
}

PPC_WEAK_FUNC(sub_82510120) {
	__imp__sub_82510120(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_825106A8) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f888
	ctx.lr = 0x825106B0;
	__savegprlr_24(ctx, base);
	// stwu r1,-176(r1)
	ea = -176 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_825106A8) {
	__imp__sub_825106A8(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82510C38) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f898
	ctx.lr = 0x82510C40;
	__savegprlr_28(ctx, base);
	// stwu r1,-192(r1)
	ea = -192 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_82510C38) {
	__imp__sub_82510C38(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82511440) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f874
	ctx.lr = 0x82511448;
	__savegprlr_19(ctx, base);
	// stwu r1,-288(r1)
	ea = -288 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_82511440) {
	__imp__sub_82511440(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82511588) {
	PPC_FUNC_PROLOGUE();
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f86c
	ctx.lr = 0x82511590;
	__savegprlr_17(ctx, base);
}

PPC_WEAK_FUNC(sub_82511588) {
	__imp__sub_82511588(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82511978) {
	PPC_FUNC_PROLOGUE();
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f870
	ctx.lr = 0x82511980;
	__savegprlr_18(ctx, base);
}

PPC_WEAK_FUNC(sub_82511978) {
	__imp__sub_82511978(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82511D30) {
	PPC_FUNC_PROLOGUE();
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f870
	ctx.lr = 0x82511D38;
	__savegprlr_18(ctx, base);
}

PPC_WEAK_FUNC(sub_82511D30) {
	__imp__sub_82511D30(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82512098) {
	PPC_FUNC_PROLOGUE();
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f87c
	ctx.lr = 0x825120A0;
	__savegprlr_21(ctx, base);
}

PPC_WEAK_FUNC(sub_82512098) {
	__imp__sub_82512098(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_825122A0) {
	PPC_FUNC_PROLOGUE();
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f868
	ctx.lr = 0x825122A8;
	__savegprlr_16(ctx, base);
}

PPC_WEAK_FUNC(sub_825122A0) {
	__imp__sub_825122A0(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_825124E8) {
	PPC_FUNC_PROLOGUE();
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f87c
	ctx.lr = 0x825124F0;
	__savegprlr_21(ctx, base);
}

PPC_WEAK_FUNC(sub_825124E8) {
	__imp__sub_825124E8(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82512720) {
	PPC_FUNC_PROLOGUE();
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f864
	ctx.lr = 0x82512728;
	__savegprlr_15(ctx, base);
}

PPC_WEAK_FUNC(sub_82512720) {
	__imp__sub_82512720(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82512998) {
	PPC_FUNC_PROLOGUE();
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f890
	ctx.lr = 0x825129A0;
	__savegprlr_26(ctx, base);
}

PPC_WEAK_FUNC(sub_82512998) {
	__imp__sub_82512998(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82512B08) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// stw r12,-8(r1)
	PPC_STORE_U32(ctx.r1.u32 + -8, ctx.r12.u32);
	// std r30,-24(r1)
	PPC_STORE_U64(ctx.r1.u32 + -24, ctx.r30.u64);
	// std r31,-16(r1)
	PPC_STORE_U64(ctx.r1.u32 + -16, ctx.r31.u64);
	// stwu r1,-112(r1)
	ea = -112 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_82512B08) {
	__imp__sub_82512B08(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82512C38) {
	PPC_FUNC_PROLOGUE();
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f898
	ctx.lr = 0x82512C40;
	__savegprlr_28(ctx, base);
}

PPC_WEAK_FUNC(sub_82512C38) {
	__imp__sub_82512C38(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82512D28) {
	PPC_FUNC_PROLOGUE();
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f898
	ctx.lr = 0x82512D30;
	__savegprlr_28(ctx, base);
}

PPC_WEAK_FUNC(sub_82512D28) {
	__imp__sub_82512D28(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82512DC8) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f88c
	ctx.lr = 0x82512DD0;
	__savegprlr_25(ctx, base);
	// stwu r1,-144(r1)
	ea = -144 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_82512DC8) {
	__imp__sub_82512DC8(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82512FD8) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f88c
	ctx.lr = 0x82512FE0;
	__savegprlr_25(ctx, base);
	// stwu r1,-144(r1)
	ea = -144 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_82512FD8) {
	__imp__sub_82512FD8(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82513050) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f890
	ctx.lr = 0x82513058;
	__savegprlr_26(ctx, base);
	// stwu r1,-144(r1)
	ea = -144 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_82513050) {
	__imp__sub_82513050(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_825130C8) {
	PPC_FUNC_PROLOGUE();
	// std r30,-16(r1)
	PPC_STORE_U64(ctx.r1.u32 + -16, ctx.r30.u64);
	// std r31,-8(r1)
	PPC_STORE_U64(ctx.r1.u32 + -8, ctx.r31.u64);
}

PPC_WEAK_FUNC(sub_825130C8) {
	__imp__sub_825130C8(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82513270) {
	PPC_FUNC_PROLOGUE();
	// std r30,-16(r1)
	PPC_STORE_U64(ctx.r1.u32 + -16, ctx.r30.u64);
	// std r31,-8(r1)
	PPC_STORE_U64(ctx.r1.u32 + -8, ctx.r31.u64);
}

PPC_WEAK_FUNC(sub_82513270) {
	__imp__sub_82513270(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82513418) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f870
	ctx.lr = 0x82513420;
	__savegprlr_18(ctx, base);
	// stwu r1,-208(r1)
	ea = -208 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_82513418) {
	__imp__sub_82513418(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_825136A8) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f880
	ctx.lr = 0x825136B0;
	__savegprlr_22(ctx, base);
	// stwu r1,-176(r1)
	ea = -176 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_825136A8) {
	__imp__sub_825136A8(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82513850) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f868
	ctx.lr = 0x82513858;
	__savegprlr_16(ctx, base);
	// stwu r1,-224(r1)
	ea = -224 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_82513850) {
	__imp__sub_82513850(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82514010) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f870
	ctx.lr = 0x82514018;
	__savegprlr_18(ctx, base);
	// stwu r1,-208(r1)
	ea = -208 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_82514010) {
	__imp__sub_82514010(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_825146A8) {
	PPC_FUNC_PROLOGUE();
	// std r31,-8(r1)
	PPC_STORE_U64(ctx.r1.u32 + -8, ctx.r31.u64);
}

PPC_WEAK_FUNC(sub_825146A8) {
	__imp__sub_825146A8(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82514760) {
	PPC_FUNC_PROLOGUE();
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f894
	ctx.lr = 0x82514768;
	__savegprlr_27(ctx, base);
}

PPC_WEAK_FUNC(sub_82514760) {
	__imp__sub_82514760(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_825148E0) {
	PPC_FUNC_PROLOGUE();
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f894
	ctx.lr = 0x825148E8;
	__savegprlr_27(ctx, base);
}

PPC_WEAK_FUNC(sub_825148E0) {
	__imp__sub_825148E0(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82514A60) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f878
	ctx.lr = 0x82514A68;
	__savegprlr_20(ctx, base);
	// stwu r1,-224(r1)
	ea = -224 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_82514A60) {
	__imp__sub_82514A60(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82514C18) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f864
	ctx.lr = 0x82514C20;
	__savegprlr_15(ctx, base);
	// stwu r1,-272(r1)
	ea = -272 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_82514C18) {
	__imp__sub_82514C18(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82514F70) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f878
	ctx.lr = 0x82514F78;
	__savegprlr_20(ctx, base);
	// stwu r1,-224(r1)
	ea = -224 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_82514F70) {
	__imp__sub_82514F70(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_825152B0) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f884
	ctx.lr = 0x825152B8;
	__savegprlr_23(ctx, base);
	// stwu r1,-192(r1)
	ea = -192 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_825152B0) {
	__imp__sub_825152B0(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82515440) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f87c
	ctx.lr = 0x82515448;
	__savegprlr_21(ctx, base);
	// stwu r1,-208(r1)
	ea = -208 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_82515440) {
	__imp__sub_82515440(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82515760) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f870
	ctx.lr = 0x82515768;
	__savegprlr_18(ctx, base);
	// stwu r1,-1104(r1)
	ea = -1104 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_82515760) {
	__imp__sub_82515760(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82515A78) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f864
	ctx.lr = 0x82515A80;
	__savegprlr_15(ctx, base);
	// stwu r1,-1152(r1)
	ea = -1152 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_82515A78) {
	__imp__sub_82515A78(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_825160E8) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f874
	ctx.lr = 0x825160F0;
	__savegprlr_19(ctx, base);
	// stwu r1,-272(r1)
	ea = -272 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_825160E8) {
	__imp__sub_825160E8(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82516720) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f874
	ctx.lr = 0x82516728;
	__savegprlr_19(ctx, base);
	// stwu r1,-272(r1)
	ea = -272 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_82516720) {
	__imp__sub_82516720(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82516D98) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f88c
	ctx.lr = 0x82516DA0;
	__savegprlr_25(ctx, base);
	// stwu r1,-144(r1)
	ea = -144 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_82516D98) {
	__imp__sub_82516D98(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82516E60) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f888
	ctx.lr = 0x82516E68;
	__savegprlr_24(ctx, base);
	// stwu r1,-160(r1)
	ea = -160 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_82516E60) {
	__imp__sub_82516E60(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82516F28) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f86c
	ctx.lr = 0x82516F30;
	__savegprlr_17(ctx, base);
	// stwu r1,-272(r1)
	ea = -272 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_82516F28) {
	__imp__sub_82516F28(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82517350) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f890
	ctx.lr = 0x82517358;
	__savegprlr_26(ctx, base);
	// stwu r1,-160(r1)
	ea = -160 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_82517350) {
	__imp__sub_82517350(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_825184A8) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f88c
	ctx.lr = 0x825184B0;
	__savegprlr_25(ctx, base);
	// stwu r1,-208(r1)
	ea = -208 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_825184A8) {
	__imp__sub_825184A8(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82518648) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f898
	ctx.lr = 0x82518650;
	__savegprlr_28(ctx, base);
	// stwu r1,-128(r1)
	ea = -128 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_82518648) {
	__imp__sub_82518648(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_825186D8) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f87c
	ctx.lr = 0x825186E0;
	__savegprlr_21(ctx, base);
	// stwu r1,-208(r1)
	ea = -208 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_825186D8) {
	__imp__sub_825186D8(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82518B60) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f87c
	ctx.lr = 0x82518B68;
	__savegprlr_21(ctx, base);
	// stwu r1,-208(r1)
	ea = -208 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_82518B60) {
	__imp__sub_82518B60(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82518EA8) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f87c
	ctx.lr = 0x82518EB0;
	__savegprlr_21(ctx, base);
	// stwu r1,-208(r1)
	ea = -208 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_82518EA8) {
	__imp__sub_82518EA8(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82519330) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f880
	ctx.lr = 0x82519338;
	__savegprlr_22(ctx, base);
	// stwu r1,-208(r1)
	ea = -208 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_82519330) {
	__imp__sub_82519330(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_825196A8) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f87c
	ctx.lr = 0x825196B0;
	__savegprlr_21(ctx, base);
	// stwu r1,-208(r1)
	ea = -208 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_825196A8) {
	__imp__sub_825196A8(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_825198B8) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f890
	ctx.lr = 0x825198C0;
	__savegprlr_26(ctx, base);
	// stwu r1,-144(r1)
	ea = -144 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_825198B8) {
	__imp__sub_825198B8(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82519958) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f87c
	ctx.lr = 0x82519960;
	__savegprlr_21(ctx, base);
	// stwu r1,-208(r1)
	ea = -208 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_82519958) {
	__imp__sub_82519958(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82519C88) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f880
	ctx.lr = 0x82519C90;
	__savegprlr_22(ctx, base);
	// stwu r1,-208(r1)
	ea = -208 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_82519C88) {
	__imp__sub_82519C88(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82519F30) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f880
	ctx.lr = 0x82519F38;
	__savegprlr_22(ctx, base);
	// stwu r1,-208(r1)
	ea = -208 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_82519F30) {
	__imp__sub_82519F30(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_8251A210) {
	PPC_FUNC_PROLOGUE();
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f89c
	ctx.lr = 0x8251A218;
	__savegprlr_29(ctx, base);
}

PPC_WEAK_FUNC(sub_8251A210) {
	__imp__sub_8251A210(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_8251A320) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f884
	ctx.lr = 0x8251A328;
	__savegprlr_23(ctx, base);
	// stwu r1,-160(r1)
	ea = -160 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_8251A320) {
	__imp__sub_8251A320(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_8251A808) {
	PPC_FUNC_PROLOGUE();
	// std r30,-16(r1)
	PPC_STORE_U64(ctx.r1.u32 + -16, ctx.r30.u64);
	// std r31,-8(r1)
	PPC_STORE_U64(ctx.r1.u32 + -8, ctx.r31.u64);
}

PPC_WEAK_FUNC(sub_8251A808) {
	__imp__sub_8251A808(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_8251A9C0) {
	PPC_FUNC_PROLOGUE();
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f890
	ctx.lr = 0x8251A9C8;
	__savegprlr_26(ctx, base);
}

PPC_WEAK_FUNC(sub_8251A9C0) {
	__imp__sub_8251A9C0(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_8251AAA0) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f868
	ctx.lr = 0x8251AAA8;
	__savegprlr_16(ctx, base);
	// stwu r1,-224(r1)
	ea = -224 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_8251AAA0) {
	__imp__sub_8251AAA0(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_8251AEE8) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f878
	ctx.lr = 0x8251AEF0;
	__savegprlr_20(ctx, base);
	// stwu r1,-192(r1)
	ea = -192 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_8251AEE8) {
	__imp__sub_8251AEE8(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_8251C340) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f884
	ctx.lr = 0x8251C348;
	__savegprlr_23(ctx, base);
	// stwu r1,-160(r1)
	ea = -160 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_8251C340) {
	__imp__sub_8251C340(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_8251C8A0) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f860
	ctx.lr = 0x8251C8A8;
	sub_8242F860(ctx, base);
	// stwu r1,-240(r1)
	ea = -240 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_8251C8A0) {
	__imp__sub_8251C8A0(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_8251D680) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f88c
	ctx.lr = 0x8251D688;
	__savegprlr_25(ctx, base);
	// stwu r1,-224(r1)
	ea = -224 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_8251D680) {
	__imp__sub_8251D680(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_8251FBA8) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f898
	ctx.lr = 0x8251FBB0;
	__savegprlr_28(ctx, base);
	// stwu r1,-128(r1)
	ea = -128 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_8251FBA8) {
	__imp__sub_8251FBA8(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_8251FC90) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f87c
	ctx.lr = 0x8251FC98;
	__savegprlr_21(ctx, base);
	// stwu r1,-208(r1)
	ea = -208 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_8251FC90) {
	__imp__sub_8251FC90(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_8251FE38) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f87c
	ctx.lr = 0x8251FE40;
	__savegprlr_21(ctx, base);
	// stwu r1,-224(r1)
	ea = -224 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_8251FE38) {
	__imp__sub_8251FE38(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_8251FFC8) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f894
	ctx.lr = 0x8251FFD0;
	__savegprlr_27(ctx, base);
	// stwu r1,-208(r1)
	ea = -208 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_8251FFC8) {
	__imp__sub_8251FFC8(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_825207C0) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f874
	ctx.lr = 0x825207C8;
	__savegprlr_19(ctx, base);
	// stwu r1,-976(r1)
	ea = -976 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_825207C0) {
	__imp__sub_825207C0(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82520B28) {
	PPC_FUNC_PROLOGUE();
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f888
	ctx.lr = 0x82520B30;
	__savegprlr_24(ctx, base);
}

PPC_WEAK_FUNC(sub_82520B28) {
	__imp__sub_82520B28(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82520D90) {
	PPC_FUNC_PROLOGUE();
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f884
	ctx.lr = 0x82520D98;
	__savegprlr_23(ctx, base);
}

PPC_WEAK_FUNC(sub_82520D90) {
	__imp__sub_82520D90(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82521388) {
	PPC_FUNC_PROLOGUE();
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f86c
	ctx.lr = 0x82521390;
	__savegprlr_17(ctx, base);
}

PPC_WEAK_FUNC(sub_82521388) {
	__imp__sub_82521388(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_825219E8) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f860
	ctx.lr = 0x825219F0;
	sub_8242F860(ctx, base);
	// stwu r1,-240(r1)
	ea = -240 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_825219E8) {
	__imp__sub_825219E8(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82522DB8) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f860
	ctx.lr = 0x82522DC0;
	sub_8242F860(ctx, base);
	// stwu r1,-256(r1)
	ea = -256 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_82522DB8) {
	__imp__sub_82522DB8(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_825231F8) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f860
	ctx.lr = 0x82523200;
	sub_8242F860(ctx, base);
	// stwu r1,-336(r1)
	ea = -336 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_825231F8) {
	__imp__sub_825231F8(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82523738) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f884
	ctx.lr = 0x82523740;
	__savegprlr_23(ctx, base);
	// stwu r1,-160(r1)
	ea = -160 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_82523738) {
	__imp__sub_82523738(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82523C48) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f860
	ctx.lr = 0x82523C50;
	sub_8242F860(ctx, base);
	// stwu r1,-256(r1)
	ea = -256 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_82523C48) {
	__imp__sub_82523C48(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82524728) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f874
	ctx.lr = 0x82524730;
	__savegprlr_19(ctx, base);
	// stwu r1,-192(r1)
	ea = -192 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_82524728) {
	__imp__sub_82524728(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82524EC0) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f890
	ctx.lr = 0x82524EC8;
	__savegprlr_26(ctx, base);
	// stwu r1,-144(r1)
	ea = -144 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_82524EC0) {
	__imp__sub_82524EC0(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82524F58) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f898
	ctx.lr = 0x82524F60;
	__savegprlr_28(ctx, base);
	// stwu r1,-144(r1)
	ea = -144 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_82524F58) {
	__imp__sub_82524F58(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82525090) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f884
	ctx.lr = 0x82525098;
	__savegprlr_23(ctx, base);
	// stwu r1,-192(r1)
	ea = -192 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_82525090) {
	__imp__sub_82525090(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82525248) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f87c
	ctx.lr = 0x82525250;
	__savegprlr_21(ctx, base);
	// stwu r1,-208(r1)
	ea = -208 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_82525248) {
	__imp__sub_82525248(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_825254D8) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f87c
	ctx.lr = 0x825254E0;
	__savegprlr_21(ctx, base);
	// stwu r1,-208(r1)
	ea = -208 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_825254D8) {
	__imp__sub_825254D8(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82525768) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f880
	ctx.lr = 0x82525770;
	__savegprlr_22(ctx, base);
	// stwu r1,-176(r1)
	ea = -176 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_82525768) {
	__imp__sub_82525768(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82528048) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f87c
	ctx.lr = 0x82528050;
	__savegprlr_21(ctx, base);
	// stwu r1,-176(r1)
	ea = -176 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_82528048) {
	__imp__sub_82528048(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_825282A8) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f884
	ctx.lr = 0x825282B0;
	__savegprlr_23(ctx, base);
	// stwu r1,-160(r1)
	ea = -160 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_825282A8) {
	__imp__sub_825282A8(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82528C50) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f884
	ctx.lr = 0x82528C58;
	__savegprlr_23(ctx, base);
	// stwu r1,-160(r1)
	ea = -160 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_82528C50) {
	__imp__sub_82528C50(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82529770) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f88c
	ctx.lr = 0x82529778;
	__savegprlr_25(ctx, base);
	// stwu r1,-144(r1)
	ea = -144 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_82529770) {
	__imp__sub_82529770(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82529B58) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f88c
	ctx.lr = 0x82529B60;
	__savegprlr_25(ctx, base);
	// stwu r1,-144(r1)
	ea = -144 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_82529B58) {
	__imp__sub_82529B58(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82529F40) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f860
	ctx.lr = 0x82529F48;
	sub_8242F860(ctx, base);
	// stwu r1,-352(r1)
	ea = -352 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_82529F40) {
	__imp__sub_82529F40(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_8252A890) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f88c
	ctx.lr = 0x8252A898;
	__savegprlr_25(ctx, base);
	// stwu r1,-208(r1)
	ea = -208 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_8252A890) {
	__imp__sub_8252A890(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_8252AD00) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f88c
	ctx.lr = 0x8252AD08;
	__savegprlr_25(ctx, base);
	// stwu r1,-208(r1)
	ea = -208 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_8252AD00) {
	__imp__sub_8252AD00(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_8252B1A0) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f880
	ctx.lr = 0x8252B1A8;
	__savegprlr_22(ctx, base);
	// stwu r1,-176(r1)
	ea = -176 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_8252B1A0) {
	__imp__sub_8252B1A0(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_8252B8C0) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f894
	ctx.lr = 0x8252B8C8;
	__savegprlr_27(ctx, base);
	// stwu r1,-192(r1)
	ea = -192 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_8252B8C0) {
	__imp__sub_8252B8C0(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_8252BD58) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f87c
	ctx.lr = 0x8252BD60;
	__savegprlr_21(ctx, base);
	// stwu r1,-192(r1)
	ea = -192 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_8252BD58) {
	__imp__sub_8252BD58(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_8252BFF0) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f860
	ctx.lr = 0x8252BFF8;
	sub_8242F860(ctx, base);
	// stwu r1,-352(r1)
	ea = -352 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_8252BFF0) {
	__imp__sub_8252BFF0(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_8252E080) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f860
	ctx.lr = 0x8252E088;
	sub_8242F860(ctx, base);
	// stwu r1,-1056(r1)
	ea = -1056 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_8252E080) {
	__imp__sub_8252E080(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_8252ECB8) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f870
	ctx.lr = 0x8252ECC0;
	__savegprlr_18(ctx, base);
	// stwu r1,-208(r1)
	ea = -208 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_8252ECB8) {
	__imp__sub_8252ECB8(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_8252F880) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f860
	ctx.lr = 0x8252F888;
	sub_8242F860(ctx, base);
	// stwu r1,-416(r1)
	ea = -416 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_8252F880) {
	__imp__sub_8252F880(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82530668) {
	PPC_FUNC_PROLOGUE();
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f890
	ctx.lr = 0x82530670;
	__savegprlr_26(ctx, base);
}

PPC_WEAK_FUNC(sub_82530668) {
	__imp__sub_82530668(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82531970) {
	PPC_FUNC_PROLOGUE();
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f898
	ctx.lr = 0x82531978;
	__savegprlr_28(ctx, base);
}

PPC_WEAK_FUNC(sub_82531970) {
	__imp__sub_82531970(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82531C18) {
	PPC_FUNC_PROLOGUE();
	// std r31,-8(r1)
	PPC_STORE_U64(ctx.r1.u32 + -8, ctx.r31.u64);
}

PPC_WEAK_FUNC(sub_82531C18) {
	__imp__sub_82531C18(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82531D60) {
	PPC_FUNC_PROLOGUE();
	// std r31,-8(r1)
	PPC_STORE_U64(ctx.r1.u32 + -8, ctx.r31.u64);
}

PPC_WEAK_FUNC(sub_82531D60) {
	__imp__sub_82531D60(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82532530) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f860
	ctx.lr = 0x82532538;
	sub_8242F860(ctx, base);
	// stwu r1,-1440(r1)
	ea = -1440 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_82532530) {
	__imp__sub_82532530(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_825337A8) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f880
	ctx.lr = 0x825337B0;
	__savegprlr_22(ctx, base);
	// stwu r1,-176(r1)
	ea = -176 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_825337A8) {
	__imp__sub_825337A8(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_825340F0) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f884
	ctx.lr = 0x825340F8;
	__savegprlr_23(ctx, base);
	// stwu r1,-176(r1)
	ea = -176 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_825340F0) {
	__imp__sub_825340F0(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82534F28) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f860
	ctx.lr = 0x82534F30;
	sub_8242F860(ctx, base);
	// stwu r1,-416(r1)
	ea = -416 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_82534F28) {
	__imp__sub_82534F28(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82535BB0) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f888
	ctx.lr = 0x82535BB8;
	__savegprlr_24(ctx, base);
	// stwu r1,-176(r1)
	ea = -176 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_82535BB0) {
	__imp__sub_82535BB0(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82535F98) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f870
	ctx.lr = 0x82535FA0;
	__savegprlr_18(ctx, base);
	// stwu r1,-224(r1)
	ea = -224 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_82535F98) {
	__imp__sub_82535F98(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_825366C0) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f884
	ctx.lr = 0x825366C8;
	__savegprlr_23(ctx, base);
	// stwu r1,-288(r1)
	ea = -288 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_825366C0) {
	__imp__sub_825366C0(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82536840) {
	PPC_FUNC_PROLOGUE();
	// std r31,-8(r1)
	PPC_STORE_U64(ctx.r1.u32 + -8, ctx.r31.u64);
}

PPC_WEAK_FUNC(sub_82536840) {
	__imp__sub_82536840(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82536AA8) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f860
	ctx.lr = 0x82536AB0;
	sub_8242F860(ctx, base);
	// stwu r1,-256(r1)
	ea = -256 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_82536AA8) {
	__imp__sub_82536AA8(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82536FD0) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f88c
	ctx.lr = 0x82536FD8;
	__savegprlr_25(ctx, base);
	// stwu r1,-176(r1)
	ea = -176 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_82536FD0) {
	__imp__sub_82536FD0(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_825372A0) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f878
	ctx.lr = 0x825372A8;
	__savegprlr_20(ctx, base);
	// stwu r1,-192(r1)
	ea = -192 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_825372A0) {
	__imp__sub_825372A0(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82537DC0) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f860
	ctx.lr = 0x82537DC8;
	sub_8242F860(ctx, base);
	// stwu r1,-352(r1)
	ea = -352 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_82537DC0) {
	__imp__sub_82537DC0(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82538538) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f860
	ctx.lr = 0x82538540;
	sub_8242F860(ctx, base);
	// stwu r1,-352(r1)
	ea = -352 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_82538538) {
	__imp__sub_82538538(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82538F18) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f860
	ctx.lr = 0x82538F20;
	sub_8242F860(ctx, base);
	// stwu r1,-1216(r1)
	ea = -1216 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_82538F18) {
	__imp__sub_82538F18(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_8253A3D8) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f860
	ctx.lr = 0x8253A3E0;
	sub_8242F860(ctx, base);
	// stwu r1,-384(r1)
	ea = -384 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_8253A3D8) {
	__imp__sub_8253A3D8(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_8253C348) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f89c
	ctx.lr = 0x8253C350;
	__savegprlr_29(ctx, base);
	// stwu r1,-176(r1)
	ea = -176 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_8253C348) {
	__imp__sub_8253C348(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_8253C478) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f860
	ctx.lr = 0x8253C480;
	sub_8242F860(ctx, base);
	// stwu r1,-640(r1)
	ea = -640 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_8253C478) {
	__imp__sub_8253C478(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_8253E1B0) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f888
	ctx.lr = 0x8253E1B8;
	__savegprlr_24(ctx, base);
	// stwu r1,-192(r1)
	ea = -192 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_8253E1B0) {
	__imp__sub_8253E1B0(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_8253E428) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f88c
	ctx.lr = 0x8253E430;
	__savegprlr_25(ctx, base);
	// stwu r1,-160(r1)
	ea = -160 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_8253E428) {
	__imp__sub_8253E428(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_8253E568) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f88c
	ctx.lr = 0x8253E570;
	__savegprlr_25(ctx, base);
	// stwu r1,-160(r1)
	ea = -160 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_8253E568) {
	__imp__sub_8253E568(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_8253E6E8) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f888
	ctx.lr = 0x8253E6F0;
	__savegprlr_24(ctx, base);
	// stwu r1,-160(r1)
	ea = -160 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_8253E6E8) {
	__imp__sub_8253E6E8(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_8253E960) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f898
	ctx.lr = 0x8253E968;
	__savegprlr_28(ctx, base);
	// stwu r1,-128(r1)
	ea = -128 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_8253E960) {
	__imp__sub_8253E960(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_8253EA70) {
	PPC_FUNC_PROLOGUE();
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f894
	ctx.lr = 0x8253EA78;
	__savegprlr_27(ctx, base);
}

PPC_WEAK_FUNC(sub_8253EA70) {
	__imp__sub_8253EA70(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_8253ECA8) {
	PPC_FUNC_PROLOGUE();
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f898
	ctx.lr = 0x8253ECB0;
	__savegprlr_28(ctx, base);
}

PPC_WEAK_FUNC(sub_8253ECA8) {
	__imp__sub_8253ECA8(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_8253EE68) {
	PPC_FUNC_PROLOGUE();
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f888
	ctx.lr = 0x8253EE70;
	__savegprlr_24(ctx, base);
}

PPC_WEAK_FUNC(sub_8253EE68) {
	__imp__sub_8253EE68(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_8253F088) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f878
	ctx.lr = 0x8253F090;
	__savegprlr_20(ctx, base);
	// stwu r1,-208(r1)
	ea = -208 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_8253F088) {
	__imp__sub_8253F088(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_8253F358) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f878
	ctx.lr = 0x8253F360;
	__savegprlr_20(ctx, base);
	// stwu r1,-192(r1)
	ea = -192 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_8253F358) {
	__imp__sub_8253F358(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_8253F478) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f86c
	ctx.lr = 0x8253F480;
	__savegprlr_17(ctx, base);
	// stwu r1,-208(r1)
	ea = -208 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_8253F478) {
	__imp__sub_8253F478(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_8253FA40) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f874
	ctx.lr = 0x8253FA48;
	__savegprlr_19(ctx, base);
	// stwu r1,-192(r1)
	ea = -192 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_8253FA40) {
	__imp__sub_8253FA40(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_8253FC70) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f894
	ctx.lr = 0x8253FC78;
	__savegprlr_27(ctx, base);
	// stwu r1,-128(r1)
	ea = -128 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_8253FC70) {
	__imp__sub_8253FC70(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_8253FED8) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f890
	ctx.lr = 0x8253FEE0;
	__savegprlr_26(ctx, base);
	// stwu r1,-176(r1)
	ea = -176 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_8253FED8) {
	__imp__sub_8253FED8(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_8253FFA0) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f890
	ctx.lr = 0x8253FFA8;
	__savegprlr_26(ctx, base);
	// stwu r1,-176(r1)
	ea = -176 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_8253FFA0) {
	__imp__sub_8253FFA0(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82540140) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f860
	ctx.lr = 0x82540148;
	sub_8242F860(ctx, base);
	// stwu r1,-336(r1)
	ea = -336 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_82540140) {
	__imp__sub_82540140(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82542340) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f860
	ctx.lr = 0x82542348;
	sub_8242F860(ctx, base);
	// stwu r1,-336(r1)
	ea = -336 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_82542340) {
	__imp__sub_82542340(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82542870) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f860
	ctx.lr = 0x82542878;
	sub_8242F860(ctx, base);
	// stwu r1,-336(r1)
	ea = -336 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_82542870) {
	__imp__sub_82542870(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82544910) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f860
	ctx.lr = 0x82544918;
	sub_8242F860(ctx, base);
	// stwu r1,-352(r1)
	ea = -352 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_82544910) {
	__imp__sub_82544910(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82546EF0) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f860
	ctx.lr = 0x82546EF8;
	sub_8242F860(ctx, base);
	// stwu r1,-352(r1)
	ea = -352 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_82546EF0) {
	__imp__sub_82546EF0(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82547748) {
	PPC_FUNC_PROLOGUE();
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f898
	ctx.lr = 0x82547750;
	__savegprlr_28(ctx, base);
}

PPC_WEAK_FUNC(sub_82547748) {
	__imp__sub_82547748(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82547860) {
	PPC_FUNC_PROLOGUE();
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f880
	ctx.lr = 0x82547868;
	__savegprlr_22(ctx, base);
}

PPC_WEAK_FUNC(sub_82547860) {
	__imp__sub_82547860(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82547CC0) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f894
	ctx.lr = 0x82547CC8;
	__savegprlr_27(ctx, base);
	// stwu r1,-192(r1)
	ea = -192 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_82547CC0) {
	__imp__sub_82547CC0(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_825483B0) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f884
	ctx.lr = 0x825483B8;
	__savegprlr_23(ctx, base);
	// stwu r1,-160(r1)
	ea = -160 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_825483B0) {
	__imp__sub_825483B0(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82548708) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f894
	ctx.lr = 0x82548710;
	__savegprlr_27(ctx, base);
	// stwu r1,-128(r1)
	ea = -128 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_82548708) {
	__imp__sub_82548708(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_825487A8) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f898
	ctx.lr = 0x825487B0;
	__savegprlr_28(ctx, base);
	// stwu r1,-128(r1)
	ea = -128 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_825487A8) {
	__imp__sub_825487A8(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82548AE8) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f860
	ctx.lr = 0x82548AF0;
	sub_8242F860(ctx, base);
	// stwu r1,-272(r1)
	ea = -272 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_82548AE8) {
	__imp__sub_82548AE8(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_825496A0) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// stw r12,-8(r1)
	PPC_STORE_U32(ctx.r1.u32 + -8, ctx.r12.u32);
	// std r30,-24(r1)
	PPC_STORE_U64(ctx.r1.u32 + -24, ctx.r30.u64);
	// std r31,-16(r1)
	PPC_STORE_U64(ctx.r1.u32 + -16, ctx.r31.u64);
	// stwu r1,-112(r1)
	ea = -112 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_825496A0) {
	__imp__sub_825496A0(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82549778) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f860
	ctx.lr = 0x82549780;
	sub_8242F860(ctx, base);
	// stwu r1,-240(r1)
	ea = -240 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_82549778) {
	__imp__sub_82549778(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_8254AD88) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f86c
	ctx.lr = 0x8254AD90;
	__savegprlr_17(ctx, base);
	// stwu r1,-272(r1)
	ea = -272 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_8254AD88) {
	__imp__sub_8254AD88(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_8254B080) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f860
	ctx.lr = 0x8254B088;
	sub_8242F860(ctx, base);
	// stwu r1,-400(r1)
	ea = -400 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_8254B080) {
	__imp__sub_8254B080(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_8254BFF0) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f860
	ctx.lr = 0x8254BFF8;
	sub_8242F860(ctx, base);
	// stwu r1,-288(r1)
	ea = -288 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_8254BFF0) {
	__imp__sub_8254BFF0(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_8254C598) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f860
	ctx.lr = 0x8254C5A0;
	sub_8242F860(ctx, base);
	// stwu r1,-256(r1)
	ea = -256 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_8254C598) {
	__imp__sub_8254C598(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_8254DA10) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f860
	ctx.lr = 0x8254DA18;
	sub_8242F860(ctx, base);
	// stwu r1,-384(r1)
	ea = -384 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_8254DA10) {
	__imp__sub_8254DA10(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_8254ECD8) {
	PPC_FUNC_PROLOGUE();
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f860
	ctx.lr = 0x8254ECE0;
	sub_8242F860(ctx, base);
}

PPC_WEAK_FUNC(sub_8254ECD8) {
	__imp__sub_8254ECD8(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82550210) {
	PPC_FUNC_PROLOGUE();
	// std r30,-16(r1)
	PPC_STORE_U64(ctx.r1.u32 + -16, ctx.r30.u64);
	// std r31,-8(r1)
	PPC_STORE_U64(ctx.r1.u32 + -8, ctx.r31.u64);
}

PPC_WEAK_FUNC(sub_82550210) {
	__imp__sub_82550210(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_825502D8) {
	PPC_FUNC_PROLOGUE();
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f898
	ctx.lr = 0x825502E0;
	__savegprlr_28(ctx, base);
}

PPC_WEAK_FUNC(sub_825502D8) {
	__imp__sub_825502D8(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82550430) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f884
	ctx.lr = 0x82550438;
	__savegprlr_23(ctx, base);
	// stwu r1,-160(r1)
	ea = -160 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_82550430) {
	__imp__sub_82550430(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82550750) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f880
	ctx.lr = 0x82550758;
	__savegprlr_22(ctx, base);
	// stwu r1,-176(r1)
	ea = -176 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_82550750) {
	__imp__sub_82550750(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_825509E0) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f878
	ctx.lr = 0x825509E8;
	__savegprlr_20(ctx, base);
	// stwu r1,-192(r1)
	ea = -192 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_825509E0) {
	__imp__sub_825509E0(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82550F68) {
	PPC_FUNC_PROLOGUE();
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f89c
	ctx.lr = 0x82550F70;
	__savegprlr_29(ctx, base);
}

PPC_WEAK_FUNC(sub_82550F68) {
	__imp__sub_82550F68(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82551038) {
	PPC_FUNC_PROLOGUE();
	// std r31,-8(r1)
	PPC_STORE_U64(ctx.r1.u32 + -8, ctx.r31.u64);
}

PPC_WEAK_FUNC(sub_82551038) {
	__imp__sub_82551038(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_825510F0) {
	PPC_FUNC_PROLOGUE();
	uint32_t ea{};
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// stw r12,-8(r1)
	PPC_STORE_U32(ctx.r1.u32 + -8, ctx.r12.u32);
	// std r30,-24(r1)
	PPC_STORE_U64(ctx.r1.u32 + -24, ctx.r30.u64);
	// std r31,-16(r1)
	PPC_STORE_U64(ctx.r1.u32 + -16, ctx.r31.u64);
	// stwu r1,-112(r1)
	ea = -112 + ctx.r1.u32;
	PPC_STORE_U32(ea, ctx.r1.u32);
	ctx.r1.u32 = ea;
}

PPC_WEAK_FUNC(sub_825510F0) {
	__imp__sub_825510F0(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_825511F8) {
	PPC_FUNC_PROLOGUE();
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f860
	ctx.lr = 0x82551200;
	sub_8242F860(ctx, base);
}

PPC_WEAK_FUNC(sub_825511F8) {
	__imp__sub_825511F8(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_825515E0) {
	PPC_FUNC_PROLOGUE();
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f860
	ctx.lr = 0x825515E8;
	sub_8242F860(ctx, base);
}

PPC_WEAK_FUNC(sub_825515E0) {
	__imp__sub_825515E0(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82551868) {
	PPC_FUNC_PROLOGUE();
	// mflr r12
	ctx.r12.u64 = ctx.lr;
	// bl 0x8242f888
	ctx.lr = 0x82551870;
	__savegprlr_24(ctx, base);
}

PPC_WEAK_FUNC(sub_82551868) {
	__imp__sub_82551868(ctx, base);
}

