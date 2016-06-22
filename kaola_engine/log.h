//
//  log.h
//  LearnIOS
//
//  Created by  on 15/6/4.
//  Copyright (c) 2015年 . All rights reserved.
//

#ifndef LearnIOS_log_h
#define LearnIOS_log_h

#include <stdio.h>

#define log_oc(x) NSLog(@"%s -> %s -> %d : %@", __FILE__, __FUNCTION__, __LINE__, x);

#define log_c(...) {printf("%s -> %s -> %d : ", __FILE__, __FUNCTION__, __LINE__);printf(__VA_ARGS__);printf("\n");}

// check below zero
#define check_fail_os(result)     if (0 > result){ \
NSLog(@"%s -> %s -> %d : %@", __FILE__, __FUNCTION__, __LINE__, [NSError errorWithDomain:NSOSStatusErrorDomain code:result userInfo:nil]); \
}

#define check_fail_c(result, texts) if (0 > result) {\
printf("%s -> %s -> %d : %s", __FILE__, __FUNCTION__, __LINE__, texts); \
}

// check false
#define check_true_os(result)     if (!result){ \
NSLog(@"%s -> %s -> %d : %@", __FILE__, __FUNCTION__, __LINE__, [NSError errorWithDomain:NSOSStatusErrorDomain code:result userInfo:nil]); \
}

#define check_true_c(result, texts) if (!result) {\
printf("%s -> %s -> %d : %s", __FILE__, __FUNCTION__, __LINE__, texts); \
}

// check condition
/**
 *  @author Kent, 16-02-28 16:02:45
 *
 *  如果result表达式为true，则进入错误报告流程
 *
 *  @param result
 *
 *  @return
 */
#define check_os(result)     if ((result)){ \
NSLog(@"%s -> %s -> %d : %@", __FILE__, __FUNCTION__, __LINE__, [NSError errorWithDomain:NSOSStatusErrorDomain code:result userInfo:nil]); \
}

/**
 *  @author Kent, 16-02-28 16:02:15
 *
 *  如果result表达式为true，则进入错误报告流程
 *
 *  @param result
 *  @param texts
 *
 *  @return
 */
#define check_c(result, texts) if ((result)) {\
printf("%s -> %s -> %d : %s", __FILE__, __FUNCTION__, __LINE__, texts); \
}

#endif
