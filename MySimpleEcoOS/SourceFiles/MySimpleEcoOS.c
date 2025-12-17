/*
 * <кодировка символов>
 *   Cyrillic (UTF-8 with signature) - Codepage 65001
 * </кодировка символов>
 *
 * <сводка>
 *   EcoVFB1
 * </сводка>
 *
 * <описание>
 *   Данный исходный файл является точкой входа
 * </описание>
 *
 * <автор>
 *   Copyright (c) 2018 Vladimir Bashev. All rights reserved.
 * </автор>
 *
 */


/* Eco OS */
#include "IEcoSystem1.h"
#include "IdEcoMemoryManager1.h"
#include "IdEcoMemoryManager1Lab.h"
#include "IdEcoInterfaceBus1.h"
#include "IEcoInterfaceBus1MemExt.h"
#include "IdEcoIPCCMailbox1.h"
#include "IdEcoVFB1.h"
#include "IEcoVBIOS1Video.h"

/* Начало свободного участка памяти */
extern char_t __heap_start__;

/*
 *
 * <сводка>
 *   Функция EcoMain
 * </сводка>
 *
 * <описание>
 *   Функция EcoMain - точка входа
 * </описание>
 *
 */
int16_t EcoMain(IEcoUnknown* pIUnk) {
    int16_t result = -1;
    /* Указатель на системный интерфейс */
    IEcoSystem1* pISys = 0;
    /* Указатель на интерфейс работы с системной интерфейсной шиной */
    IEcoInterfaceBus1* pIBus = 0;
    /* Указатель на интерфейс работы с памятью */
    IEcoMemoryAllocator1* pIMem = 0;
    IEcoMemoryManager1* pIMemMgr = 0;
    IEcoInterfaceBus1MemExt* pIMemExt = 0;
    /* Указатель на интерфейс для работы c буфером кадров видеоустройства */
    IEcoVFB1* pIVFB = 0;
    ECO_VFB_1_SCREEN_MODE xScreenMode = {0};
    IEcoVBIOS1Video* pIVideo = 0;

    char_t* pMem1 = 0;
    char_t* pMem2 = 0;
    char_t* pMem3 = 0;
    char_t* pGot = 0;
    char_t* pBarrier1 = 0;
    char_t* pExpect = 0;
    char_t* pPick = 0;
    char_t* pTmpA = 0;
    char_t* pTmpB = 0;

    /* Создание экземпляра интерфейсной шины */
    result = GetIEcoComponentFactoryPtr_00000000000000000000000042757331->pVTbl->Alloc(GetIEcoComponentFactoryPtr_00000000000000000000000042757331, 0, 0, &IID_IEcoInterfaceBus1, (void **)&pIBus);
    /* Проверка */
    if (result != 0 && pIBus == 0) {
        /* Освобождение в случае ошибки */
        goto Release;
    }

    /* Регистрация статического компонента для работы с памятью */
    result = pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoMemoryManager1, (IEcoUnknown*)GetIEcoComponentFactoryPtr_0000000000000000000000004D656D31);
    result = pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoMemoryManager1Lab, (IEcoUnknown*)GetIEcoComponentFactoryPtr_81589BFED0B84B1194524BEE623E1838);
    /* Проверка */
    if (result != 0) {
        /* Освобождение в случае ошибки */
        goto Release;
    }

    /* Регистрация статического компонента для работы с ящиком прошивки */
    result = pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoIPCCMailbox1, (IEcoUnknown*)GetIEcoComponentFactoryPtr_F10BC39A4F2143CF8A1E104650A2C302);
    /* Проверка */
    if (result != 0) {
        /* Освобождение в случае ошибки */
        goto Release;
    }

    /* Запрос расширения интерфейсной шины */
    result = pIBus->pVTbl->QueryInterface(pIBus, &IID_IEcoInterfaceBus1MemExt, (void**)&pIMemExt);
    if (result == 0 && pIMemExt != 0) {
        pIMemExt->pVTbl->set_Manager(pIMemExt, &CID_EcoMemoryManager1Lab);
        pIMemExt->pVTbl->set_ExpandPool(pIMemExt, 1);
    }

    /* Получение интерфейса управления памятью */
    result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoMemoryManager1Lab, 0, &IID_IEcoMemoryManager1, (void**) &pIMemMgr);
    if (result != 0 || pIMemMgr == 0) {
        /* Возврат в случае ошибки */
        return result;
    }

    /* Выделение области памяти 512 КБ */
    pIMemMgr->pVTbl->Init(pIMemMgr, &__heap_start__, 0x080000);

    /* Регистрация статического компонента для работы с VBF */
    result = pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoVFB1, (IEcoUnknown*)GetIEcoComponentFactoryPtr_939B1DCDB6404F7D9C072291AF252188);
    /* Проверка */
    if (result != 0) {
        /* Освобождение в случае ошибки */
        goto Release;
    }

    result = pIMemMgr->pVTbl->QueryInterface(pIMemMgr, &IID_IEcoMemoryAllocator1, (void**)&pIMem);
    if (result != 0 || pIMem == 0) {
        goto Release;
    }

    /* Получение интерфейса для работы с видео сервисами VBF */
    result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoVFB1, 0, &IID_IEcoVFB1, (void**) &pIVFB);
    /* Проверка */
    if (result != 0 || pIVFB == 0) {
        /* Освобождение в случае ошибки */
        goto Release;
    }

    /* Получение информации о текущем режиме экрана */
    result = pIVFB->pVTbl->get_Mode(pIVFB, &xScreenMode);
    pIVFB->pVTbl->Create(pIVFB, 0, 0, xScreenMode.Width, xScreenMode.Height);
    result = pIVFB->pVTbl->QueryInterface(pIVFB, &IID_IEcoVBIOS1Video, (void**) &pIVideo);

    pIVideo->pVTbl->WriteString(pIVideo, 0, 0, 0, 0, CHARACTER_ATTRIBUTE_FORE_COLOR_YELLOW, "--- TEST START ---", 18);

    pIVideo->pVTbl->WriteString(pIVideo, 0, 0, 0, 1, CHARACTER_ATTRIBUTE_FORE_COLOR_YELLOW, "TEST 1: Best Fit (two free blocks) minimal remainder", 52);

    pMem1 = (char_t*)pIMem->pVTbl->Alloc(pIMem, 400);
    pMem2 = (char_t*)pIMem->pVTbl->Alloc(pIMem, 64);
    pMem3 = (char_t*)pIMem->pVTbl->Alloc(pIMem, 300);
    pBarrier1 = (char_t*)pIMem->pVTbl->Alloc(pIMem, 200);

    pExpect = pMem3;

    if (pMem1 && pMem2 && pMem3 && pBarrier1) {
        pIMem->pVTbl->Free(pIMem, pMem1);
        pIMem->pVTbl->Free(pIMem, pMem3);

        pGot = (char_t*)pIMem->pVTbl->Alloc(pIMem, 280);

        if (pGot && pGot == pExpect) {
            pIVideo->pVTbl->WriteString(pIVideo, 0, 0, 0, 2, CHARACTER_ATTRIBUTE_FORE_COLOR_GREEN, "OK: best-fit selected smaller block", 35);
        } else {
            pIVideo->pVTbl->WriteString(pIVideo, 0, 0, 0, 2, CHARACTER_ATTRIBUTE_FORE_COLOR_RED, "ERR: block selection is not best-fit", 36);
        }

        if (pGot) pIMem->pVTbl->Free(pIMem, pGot);
        if (pBarrier1) pIMem->pVTbl->Free(pIMem, pBarrier1);
        if (pMem2) pIMem->pVTbl->Free(pIMem, pMem2);
    } else {
        pIVideo->pVTbl->WriteString(pIVideo, 0, 0, 0, 2, CHARACTER_ATTRIBUTE_FORE_COLOR_RED, "ERR: block selection is not best-fit", 36);
        if (pMem1) pIMem->pVTbl->Free(pIMem, pMem1);
        if (pMem2) pIMem->pVTbl->Free(pIMem, pMem2);
        if (pMem3) pIMem->pVTbl->Free(pIMem, pMem3);
        if (pBarrier1) pIMem->pVTbl->Free(pIMem, pBarrier1);
    }


    pIVideo->pVTbl->WriteString(pIVideo, 0, 0, 0, 3, CHARACTER_ATTRIBUTE_FORE_COLOR_YELLOW, "TEST 2: Best Fit full scan of free blocks", 41);

    pMem1 = (char_t*)pIMem->pVTbl->Alloc(pIMem, 520);
    pMem2 = (char_t*)pIMem->pVTbl->Alloc(pIMem, 64);

    pMem3 = (char_t*)pIMem->pVTbl->Alloc(pIMem, 340);
    pGot = (char_t*)pIMem->pVTbl->Alloc(pIMem, 64);

    pBarrier1 = (char_t*)pIMem->pVTbl->Alloc(pIMem, 260);
    pExpect = (char_t*)pIMem->pVTbl->Alloc(pIMem, 64);

    pTmpA = (char_t*)pIMem->pVTbl->Alloc(pIMem, 480);
    pTmpB = (char_t*)pIMem->pVTbl->Alloc(pIMem, 64);

    if (pMem1 && pMem2 && pMem3 && pGot && pBarrier1 && pExpect && pTmpA && pTmpB) {
        pIMem->pVTbl->Free(pIMem, pMem1);
        pIMem->pVTbl->Free(pIMem, pMem3);
        pIMem->pVTbl->Free(pIMem, pBarrier1);
        pIMem->pVTbl->Free(pIMem, pTmpA);

        pPick = (char_t*)pIMem->pVTbl->Alloc(pIMem, 240);

        if (pPick && pPick == pBarrier1) {
            pIVideo->pVTbl->WriteString(pIVideo, 0, 0, 0, 4, CHARACTER_ATTRIBUTE_FORE_COLOR_GREEN, "OK: best-fit selected optimal block", 34);
        } else {
            pIVideo->pVTbl->WriteString(pIVideo, 0, 0, 0, 4, CHARACTER_ATTRIBUTE_FORE_COLOR_RED, "ERR: non-optimal block selected", 31);
        }

        if (pPick) pIMem->pVTbl->Free(pIMem, pPick);

        if (pMem2) pIMem->pVTbl->Free(pIMem, pMem2);
        if (pGot) pIMem->pVTbl->Free(pIMem, pGot);
        if (pExpect) pIMem->pVTbl->Free(pIMem, pExpect);
        if (pTmpB) pIMem->pVTbl->Free(pIMem, pTmpB);
    } else {
        pIVideo->pVTbl->WriteString(pIVideo, 0, 0, 0, 4, CHARACTER_ATTRIBUTE_FORE_COLOR_RED, "ERR: non-optimal block selected", 31);

        if (pMem1) pIMem->pVTbl->Free(pIMem, pMem1);
        if (pMem2) pIMem->pVTbl->Free(pIMem, pMem2);
        if (pMem3) pIMem->pVTbl->Free(pIMem, pMem3);
        if (pGot) pIMem->pVTbl->Free(pIMem, pGot);
        if (pBarrier1) pIMem->pVTbl->Free(pIMem, pBarrier1);
        if (pExpect) pIMem->pVTbl->Free(pIMem, pExpect);
        if (pTmpA) pIMem->pVTbl->Free(pIMem, pTmpA);
        if (pTmpB) pIMem->pVTbl->Free(pIMem, pTmpB);
        if (pPick) pIMem->pVTbl->Free(pIMem, pPick);
    }

    pIVideo->pVTbl->WriteString(pIVideo, 0, 0, 0, 5, CHARACTER_ATTRIBUTE_FORE_COLOR_YELLOW, "TEST 3: Best Fit exact-fit priority", 35);

    pMem1 = (char_t*)pIMem->pVTbl->Alloc(pIMem, 240);
    pMem2 = (char_t*)pIMem->pVTbl->Alloc(pIMem, 64);
    pMem3 = (char_t*)pIMem->pVTbl->Alloc(pIMem, 160);
    pBarrier1 = (char_t*)pIMem->pVTbl->Alloc(pIMem, 64);

    pExpect = pMem3;

    if (pMem1 && pMem2 && pMem3 && pBarrier1) {
        pIMem->pVTbl->Free(pIMem, pMem1);
        pIMem->pVTbl->Free(pIMem, pMem3);

        pGot = (char_t*)pIMem->pVTbl->Alloc(pIMem, 160);

        if (pGot && pGot == pExpect) {
            pIVideo->pVTbl->WriteString(pIVideo, 0, 0, 0, 6, CHARACTER_ATTRIBUTE_FORE_COLOR_GREEN, "OK: exact-size block was selected", 33);
        } else {
            pIVideo->pVTbl->WriteString(pIVideo, 0, 0, 0, 6, CHARACTER_ATTRIBUTE_FORE_COLOR_RED, "ERR: exact-size block not selected", 34);
        }

        if (pGot) pIMem->pVTbl->Free(pIMem, pGot);
        if (pBarrier1) pIMem->pVTbl->Free(pIMem, pBarrier1);
        if (pMem2) pIMem->pVTbl->Free(pIMem, pMem2);
    } else {
        pIVideo->pVTbl->WriteString(pIVideo, 0, 0, 0, 6, CHARACTER_ATTRIBUTE_FORE_COLOR_RED, "ERR: exact-size block not selected", 34);
        if (pMem1) pIMem->pVTbl->Free(pIMem, pMem1);
        if (pMem2) pIMem->pVTbl->Free(pIMem, pMem2);
        if (pMem3) pIMem->pVTbl->Free(pIMem, pMem3);
        if (pBarrier1) pIMem->pVTbl->Free(pIMem, pBarrier1);
    }


    pIVideo->pVTbl->WriteString(pIVideo, 0, 0, 0, 7, CHARACTER_ATTRIBUTE_FORE_COLOR_YELLOW, "TEST 4: Best Fit uses split remainder", 37);

    pMem1 = (char_t*)pIMem->pVTbl->Alloc(pIMem, 1024);
    pMem2 = (char_t*)pIMem->pVTbl->Alloc(pIMem, 64);
    pMem3 = (char_t*)pIMem->pVTbl->Alloc(pIMem, 2048);
    pBarrier1 = (char_t*)pIMem->pVTbl->Alloc(pIMem, 64);

    pExpect = pMem3;

    if (pMem1 && pMem2 && pMem3 && pBarrier1) {
        pIMem->pVTbl->Free(pIMem, pMem1);
        pIMem->pVTbl->Free(pIMem, pMem3);

        pTmpA = (char_t*)pIMem->pVTbl->Alloc(pIMem, 512);
        pTmpB = (char_t*)pIMem->pVTbl->Alloc(pIMem, 128);

        if (pTmpA && pTmpB && pTmpA == pMem1 && pTmpB != pExpect) {
            pIVideo->pVTbl->WriteString(pIVideo, 0, 0, 0, 8, CHARACTER_ATTRIBUTE_FORE_COLOR_GREEN, "OK: reused remainder after split", 32);
        } else {
            pIVideo->pVTbl->WriteString(pIVideo, 0, 0, 0, 8, CHARACTER_ATTRIBUTE_FORE_COLOR_RED, "ERR: did not reuse split remainder", 34);
        }

        if (pTmpB) pIMem->pVTbl->Free(pIMem, pTmpB);
        if (pTmpA) pIMem->pVTbl->Free(pIMem, pTmpA);
        if (pBarrier1) pIMem->pVTbl->Free(pIMem, pBarrier1);
        if (pMem2) pIMem->pVTbl->Free(pIMem, pMem2);
    } else {
        pIVideo->pVTbl->WriteString(pIVideo, 0, 0, 0, 8, CHARACTER_ATTRIBUTE_FORE_COLOR_RED, "ERR: did not reuse split remainder", 34);
        if (pMem1) pIMem->pVTbl->Free(pIMem, pMem1);
        if (pMem2) pIMem->pVTbl->Free(pIMem, pMem2);
        if (pMem3) pIMem->pVTbl->Free(pIMem, pMem3);
        if (pBarrier1) pIMem->pVTbl->Free(pIMem, pBarrier1);
        if (pTmpA) pIMem->pVTbl->Free(pIMem, pTmpA);
        if (pTmpB) pIMem->pVTbl->Free(pIMem, pTmpB);
    }


    pIVideo->pVTbl->WriteString(pIVideo, 0, 0, 0, 9, CHARACTER_ATTRIBUTE_FORE_COLOR_YELLOW, "TEST 5: Best Fit after coalescing", 33);

    pMem1 = (char_t*)pIMem->pVTbl->Alloc(pIMem, 200);
    pMem2 = (char_t*)pIMem->pVTbl->Alloc(pIMem, 200);
    pMem3 = (char_t*)pIMem->pVTbl->Alloc(pIMem, 200);
    pGot = (char_t*)pIMem->pVTbl->Alloc(pIMem, 64);
    pBarrier1 = (char_t*)pIMem->pVTbl->Alloc(pIMem, 800);

    if (pMem1 && pMem2 && pMem3 && pGot && pBarrier1) {
        pIMem->pVTbl->Free(pIMem, pMem1);
        pIMem->pVTbl->Free(pIMem, pMem2);
        pIMem->pVTbl->Free(pIMem, pMem3);
        pIMem->pVTbl->Free(pIMem, pBarrier1);

        pPick = (char_t*)pIMem->pVTbl->Alloc(pIMem, 500);

        if (pPick && pPick == pMem1) {
            pIVideo->pVTbl->WriteString(pIVideo, 0, 0, 0, 10, CHARACTER_ATTRIBUTE_FORE_COLOR_GREEN, "OK: used merged free region", 27);
        } else {
            pIVideo->pVTbl->WriteString(pIVideo, 0, 0, 0, 10, CHARACTER_ATTRIBUTE_FORE_COLOR_RED, "ERR: merge or selection failed", 30);
        }

        if (pPick) pIMem->pVTbl->Free(pIMem, pPick);
        if (pGot) pIMem->pVTbl->Free(pIMem, pGot);
    } else {
        pIVideo->pVTbl->WriteString(pIVideo, 0, 0, 0, 10, CHARACTER_ATTRIBUTE_FORE_COLOR_RED, "ERR: merge or selection failed", 30);
        if (pMem1) pIMem->pVTbl->Free(pIMem, pMem1);
        if (pMem2) pIMem->pVTbl->Free(pIMem, pMem2);
        if (pMem3) pIMem->pVTbl->Free(pIMem, pMem3);
        if (pGot) pIMem->pVTbl->Free(pIMem, pGot);
        if (pBarrier1) pIMem->pVTbl->Free(pIMem, pBarrier1);
        if (pPick) pIMem->pVTbl->Free(pIMem, pPick);
    }


    pIVideo->pVTbl->WriteString(pIVideo, 0, 0, 0, 11, CHARACTER_ATTRIBUTE_FORE_COLOR_YELLOW, "TEST 6: Best Fit deterministic choice on equal blocks", 53);

    pMem1 = (char_t*)pIMem->pVTbl->Alloc(pIMem, 256);
    pMem2 = (char_t*)pIMem->pVTbl->Alloc(pIMem, 64);
    pMem3 = (char_t*)pIMem->pVTbl->Alloc(pIMem, 256);
    pGot = (char_t*)pIMem->pVTbl->Alloc(pIMem, 64);
    pBarrier1 = (char_t*)pIMem->pVTbl->Alloc(pIMem, 256);

    if (pMem1 && pMem2 && pMem3 && pGot && pBarrier1) {
        pIMem->pVTbl->Free(pIMem, pMem1);
        pIMem->pVTbl->Free(pIMem, pMem3);

        pTmpA = (char_t*)pIMem->pVTbl->Alloc(pIMem, 128);

        if (pTmpA && pTmpA == pMem1) {
            pIVideo->pVTbl->WriteString(pIVideo, 0, 0, 0, 12, CHARACTER_ATTRIBUTE_FORE_COLOR_GREEN, "OK: stable choice on equal blocks", 33);
        } else {
            pIVideo->pVTbl->WriteString(pIVideo, 0, 0, 0, 12, CHARACTER_ATTRIBUTE_FORE_COLOR_RED, "ERR: non-deterministic block selection", 38);
        }

        if (pTmpA) pIMem->pVTbl->Free(pIMem, pTmpA);
        if (pBarrier1) pIMem->pVTbl->Free(pIMem, pBarrier1);
        if (pMem2) pIMem->pVTbl->Free(pIMem, pMem2);
        if (pGot) pIMem->pVTbl->Free(pIMem, pGot);
    } else {
        pIVideo->pVTbl->WriteString(pIVideo, 0, 0, 0, 12, CHARACTER_ATTRIBUTE_FORE_COLOR_RED, "ERR: non-deterministic block selection", 38);
        if (pMem1) pIMem->pVTbl->Free(pIMem, pMem1);
        if (pMem2) pIMem->pVTbl->Free(pIMem, pMem2);
        if (pMem3) pIMem->pVTbl->Free(pIMem, pMem3);
        if (pGot) pIMem->pVTbl->Free(pIMem, pGot);
        if (pBarrier1) pIMem->pVTbl->Free(pIMem, pBarrier1);
        if (pTmpA) pIMem->pVTbl->Free(pIMem, pTmpA);
    }

    pIVideo->pVTbl->WriteString(pIVideo, 0, 0, 0, 13, CHARACTER_ATTRIBUTE_FORE_COLOR_YELLOW, "--- TEST END ---", 16);

    while(1) {
        asm volatile ("NOP\n\t" ::: "memory");
    }

Release:
    if (pIMemMgr != 0) pIMemMgr->pVTbl->Release(pIMemMgr);

    /* Освобождение интерфейса для работы с интерфейсной шиной */
    if (pIBus != 0) {
        pIBus->pVTbl->Release(pIBus);
    }

    /* Освобождение интерфейса работы с памятью */
    if (pIMem != 0) {
        pIMem->pVTbl->Release(pIMem);
    }

    /* Освобождение интерфейса VFB */
    if (pIVFB != 0) {
        pIVFB->pVTbl->Release(pIVFB);
    }

    /* Освобождение системного интерфейса */
    if (pISys != 0) {
        pISys->pVTbl->Release(pISys);
    }

    return result;
}

