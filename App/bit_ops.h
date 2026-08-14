
#ifndef __BIT_OPS_H__
#define __BIT_OPS_H__


#define __SET_BIT(x, n)       ((x) |= (1 << (n)))     // 将第n位置1
#define __CLEAR_BIT(x, n)     ((x) &= ~(1 << (n)))    // 将第n位置0
#define __TOGGLE_BIT(x, n)    ((x) ^= (1 << (n)))     // 反转第n位
#define __CHECK_BIT(x, n)     ((x) & (1 << (n)))      // 返回第n位的值 0/1

#define __SET_MASK(x, mask)       ((x) |= (mask))             // 按照掩码将特定位置1
#define __CLEAR_MASK(x, mask)     ((x) &= ~(mask))            // 按照掩码将特定位置0
#define __TOGGLE_MASK(x, mask)    ((x) ^= (mask))             // 按照掩码反转特定位
#define __CHECK_MASK(x, mask)     ((x) & (mask) == (mask))    // 检查源数据对应位是否都为1


#endif  /* __BIT_OPS_H__ */