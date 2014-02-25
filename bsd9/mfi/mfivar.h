/*-
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *            Copyright 1994-2011 The FreeBSD Project.
 *            All rights reserved.   
 *                 
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 *    THIS SOFTWARE IS PROVIDED BY THE FREEBSD PROJECT``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FREEBSD PROJECT OR 
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY,OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION)HOWEVER CAUSED AND ON ANY THEORY 
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation 
 * are those of the authors and should not be interpreted as representing 
 * official policies,either expressed or implied, of the FreeBSD Project.	
 *
 * Send feedback to: <megaraidfbsd@lsi.com>
 * Mail to: LSI Corporation, 1621 Barber Lane, Milpitas, CA 95035
 *    ATTN: MegaRaid FreeBSD
 *
 * FILE: mfivar.h
 *
 * DESCRIPTION:
 * This file contains constant defines and struct declarations for the
 * mfi driver.
 *
 * CHANGE HISTORY:
 * Date        By   Description
 * ----------  --  -------------------------------------------------------------
 *
 */

#ifndef _MFIVAR_H
#define _MFIVAR_H

#include <sys/cdefs.h>
__FBSDID("$FreeBSD: src/sys/dev/mfi/mfivar.h,v 1.11 2007/08/25 23:58:45 scottl Exp $");

#include <sys/lock.h>
#include <sys/sx.h>

#include <sys/types.h>
#include <sys/cdefs.h>
#include <machine/atomic.h>

/*
 * SCSI structures and definitions are used from here, but no linking
 * requirements are made to CAM.
 */
#include <cam/scsi/scsi_all.h>

struct mfi_hwcomms {
	uint32_t		hw_pi;
	uint32_t		hw_ci;
	uint32_t		hw_reply_q[1];
};
#define  MEGASAS_MAX_NAME 32
#define  MEGASAS_VERSION "4.410.01.00" 

struct mfi_softc;
struct disk;
struct ccb_hdr;

struct mfi_command {
	TAILQ_ENTRY(mfi_command) cm_link;
	time_t			cm_timestamp;
	struct mfi_softc	*cm_sc;
	union mfi_frame		*cm_frame;
	#if defined(__amd64__)
		uint64_t		cm_frame_busaddr;
	#else
	uint32_t		cm_frame_busaddr;
	#endif
	struct mfi_sense	*cm_sense;
	#if defined(__amd64__)
		uint64_t		cm_sense_busaddr;
	#else
	uint32_t		cm_sense_busaddr;
	#endif
	bus_dmamap_t		cm_dmamap;
	union mfi_sgl		*cm_sg;
	void			*cm_data;
	int			cm_len;
	int			cm_total_frame_size;
	int			cm_extra_frames;
	int			cm_flags;
#define MFI_CMD_MAPPED		(1<<0)
#define MFI_CMD_DATAIN		(1<<1)
#define MFI_CMD_DATAOUT		(1<<2)
#define MFI_CMD_COMPLETED	(1<<3)
#define MFI_CMD_POLLED		(1<<4)
#define MFI_ON_MFIQ_FREE	(1<<5)
#define MFI_ON_MFIQ_READY	(1<<6)
#define MFI_ON_MFIQ_BUSY	(1<<7)
#define MFI_ON_MFIQ_MASK	((1<<5)|(1<<6)|(1<<7))
	int			cm_aen_abort;
	uint8_t			retry_for_fw_reset;
	void			(* cm_complete)(struct mfi_command *cm);
	void			*cm_private;
	int			cm_index;
	int			cm_error;
};

struct mfi_disk {
	TAILQ_ENTRY(mfi_disk)	ld_link;
	device_t	ld_dev;
	int		ld_id;
	int		ld_unit;
	struct mfi_softc *ld_controller;
	struct mfi_ld_info	*ld_info;
	struct disk	*ld_disk;
	int		ld_flags;
#define MFI_DISK_FLAGS_OPEN	0x01
#define	MFI_DISK_FLAGS_DISABLED	0x02
};

struct mfi_system_pd {
	TAILQ_ENTRY(mfi_system_pd)	pd_link;
	device_t	pd_dev;
	int		pd_id;
	int		pd_unit;
	struct mfi_softc *pd_controller;
	struct mfi_pd_info	*pd_info;
	struct disk	*pd_disk;
	int		pd_flags;
};
struct mfi_aen {
	TAILQ_ENTRY(mfi_aen) aen_link;
	struct proc			*p;
};

struct mfi_skinny_dma_info {
	bus_dma_tag_t	dmat[514];
	bus_dmamap_t	dmamap[514];
	uint32_t	mem[514];
	int		noofmaps;
};


typedef struct {
	volatile unsigned int val;
} atomic_t;

#define atomic_read(v)	((v)->val)
#define atomic_set(v,i)	((v)->val - (i))


struct mfi_softc {
	device_t			mfi_dev;
	int				mfi_flags;
#define MFI_FLAGS_SG64		(1<<0)
#define MFI_FLAGS_QFRZN		(1<<1)
#define MFI_FLAGS_OPEN		(1<<2)
#define MFI_FLAGS_STOP		(1<<3)
#define MFI_FLAGS_1064R		(1<<4)
#define MFI_FLAGS_1078		(1<<5)
#define MFI_FLAGS_GEN2		(1<<6)
#define MFI_FLAGS_SKINNY	(1<<7)
#ifdef TBOLT_INV // Keep the following flags to ensure not to be used
#define MFI_FLAGS_TBOLT		(1<<8)
#define MFI_FLAGS_INVADER	(1<<9)
#endif 
	// Start: LSIP200113393
	bus_dma_tag_t			verbuf_h_dmat;
	bus_dmamap_t			verbuf_h_dmamap;
	bus_addr_t			verbuf_h_busaddr;		
	uint32_t				*verbuf;
	void				* kbuff_arr[MAX_IOCTL_SGE];
	bus_dma_tag_t			mfi_kbuff_arr_dmat[2];
	bus_dmamap_t			mfi_kbuff_arr_dmamap[2];
	bus_addr_t			mfi_kbuff_arr_busaddr[2];

	struct mfi_hwcomms		*mfi_comms;
	TAILQ_HEAD(,mfi_command)	mfi_free;
	TAILQ_HEAD(,mfi_command)	mfi_ready;
	TAILQ_HEAD(BUSYQ,mfi_command)	mfi_busy;
	struct bio_queue_head		mfi_bioq;
	struct mfi_qstat		mfi_qstat[MFIQ_COUNT];

	struct resource			*mfi_regs_resource;
	bus_space_handle_t		mfi_bhandle;
	bus_space_tag_t			mfi_btag;
	int				mfi_regs_rid;

	bus_dma_tag_t			mfi_parent_dmat;
	bus_dma_tag_t			mfi_buffer_dmat;

	bus_dma_tag_t			mfi_comms_dmat;
	bus_dmamap_t			mfi_comms_dmamap;
	bus_addr_t			mfi_comms_busaddr;	

	bus_dma_tag_t			mfi_frames_dmat;
	bus_dmamap_t			mfi_frames_dmamap;
	bus_addr_t			mfi_frames_busaddr;
	union mfi_frame			*mfi_frames;
	
	bus_dma_tag_t			mfi_tb_init_dmat;
	bus_dmamap_t			mfi_tb_init_dmamap;
	bus_addr_t			mfi_tb_init_busaddr;
	bus_addr_t			mfi_tb_ioc_init_busaddr;
	union mfi_frame			*mfi_tb_init;
	

	TAILQ_HEAD(,mfi_aen)		mfi_aen_pids;
	struct mfi_command		*mfi_aen_cm;
	struct mfi_command		*mfi_skinny_cm;
	uint32_t			mfi_aen_triggered;
	uint32_t			mfi_poll_waiting;
	struct selinfo			mfi_select;
	int				mfi_delete_busy_volumes;
	int				mfi_keep_deleted_volumes;
	int				mfi_detaching;

	bus_dma_tag_t			mfi_sense_dmat;
	bus_dmamap_t			mfi_sense_dmamap;
	bus_addr_t			mfi_sense_busaddr;
	struct mfi_sense		*mfi_sense;

	struct resource			*mfi_irq;
	void				*mfi_intr;
	int				mfi_irq_rid;

	struct intr_config_hook		mfi_ich;
	eventhandler_tag		eh;
	/*OCR flags*/
	atomic_t fw_reset_no_pci_access;
	uint8_t adpreset;
	uint8_t issuepend_done;
	uint8_t disableOnlineCtrlReset;
	uint32_t mfiStatus;
	uint32_t last_seq_num;
	uint32_t volatile hw_crit_error;

	/*
	 * Allocation for the command array.  Used as an indexable array to
	 * recover completed commands.
	 */
	struct mfi_command		*mfi_commands;
	/*
	 * How many commands were actually allocated
	 */
	int				mfi_total_cmds;
	/*
	 * How many commands the firmware can handle.  Also how big the reply
	 * queue is, minus 1.
	 */
	int				mfi_max_fw_cmds;
	/*
	 * How many S/G elements we'll ever actually use 
	 */
	int				mfi_max_sge;
	/*
	 * How many bytes a compound frame is, including all of the extra frames
	 * that are used for S/G elements.
	 */
	int				mfi_cmd_size;
	/*
	 * How large an S/G element is.  Used to calculate the number of single
	 * frames in a command.
	 */
	int				mfi_sge_size;
	/*
	 * Max number of sectors that the firmware allows
	 */
	uint32_t			mfi_max_io;

	TAILQ_HEAD(,mfi_disk)		mfi_ld_tqh;
	TAILQ_HEAD(,mfi_system_pd)	mfi_syspd_tqh;
	eventhandler_tag		mfi_eh;
	struct cdev			*mfi_cdev;

	TAILQ_HEAD(, ccb_hdr)		mfi_cam_ccbq;
	struct mfi_command *		(* mfi_cam_start)(void *);
	struct callout			mfi_watchdog_callout;
	struct mtx			mfi_io_lock;
	struct mtx			mfi_cmd_pool_lock;
	struct mtx			mfi_isr_lock;
	struct sx			mfi_config_lock;

	/* Controller type specific interfaces */
	void	(*mfi_enable_intr)(struct mfi_softc *sc);
	void	(*mfi_disable_intr)(struct mfi_softc *sc);
	int32_t	(*mfi_read_fw_status)(struct mfi_softc *sc);
	int	(*mfi_check_clear_intr)(struct mfi_softc *sc);
	#if defined(__amd64__)
 	void	(*mfi_issue_cmd)(struct mfi_softc *sc,uint64_t bus_add,uint32_t frame_cnt);
	#else
	void	(*mfi_issue_cmd)(struct mfi_softc *sc,uint32_t bus_add,uint32_t frame_cnt);
	#endif
	int (*mfi_adp_reset)(struct mfi_softc *sc);
	int (*mfi_adp_check_reset)(struct mfi_softc *sc);


};


// TODO find the right definition
#define XXX_MFI_CMD_OP_INIT2                    0x9


/* T10 PI defines */
//#define MR_PROT_INFO_TYPE_CONTROLLER		0x08
#define	MEGASAS_SCSI_VARIABLE_LENGTH_CMD	0x7f
#define MEGASAS_SCSI_SERVICE_ACTION_READ32	0x9
#define MEGASAS_SCSI_SERVICE_ACTION_WRITE32	0xB
#define	MEGASAS_SCSI_ADDL_CDB_LEN   		0x18
#define MEGASAS_RD_WR_PROTECT_CHECK_ALL		0x20
#define MEGASAS_RD_WR_PROTECT_CHECK_NONE	0x60
#define MEGASAS_EEDPBLOCKSIZE			512


extern int mfi_attach(struct mfi_softc *);
extern void mfi_free(struct mfi_softc *);
extern int mfi_flushcache(struct mfi_softc *);     
extern int mfi_shutdown(struct mfi_softc *);
extern void mfi_startio(struct mfi_softc *);
extern void mfi_disk_complete(struct bio *);
extern int mfi_disk_disable(struct mfi_disk *);
extern void mfi_disk_enable(struct mfi_disk *);
extern int mfi_dump_blocks(struct mfi_softc *, int id, uint64_t, void *, int);
extern int mfi_syspd_disable(struct mfi_system_pd *);
extern void mfi_syspd_enable(struct mfi_system_pd *);
extern int mfi_dump_syspd_blocks(struct mfi_softc *, int id, uint64_t, void *, int);

#define MFIQ_ADD(sc, qname)					\
	do {							\
		struct mfi_qstat *qs;				\
								\
		qs = &(sc)->mfi_qstat[qname];			\
		qs->q_length++;					\
		if (qs->q_length > qs->q_max)			\
			qs->q_max = qs->q_length;		\
	} while (0)

#define MFIQ_REMOVE(sc, qname)	(sc)->mfi_qstat[qname].q_length--

#define MFIQ_INIT(sc, qname)					\
	do {							\
		sc->mfi_qstat[qname].q_length = 0;		\
		sc->mfi_qstat[qname].q_max = 0;			\
	} while (0)

#define MFIQ_COMMAND_QUEUE(name, index)					\
	static __inline void						\
	mfi_initq_ ## name (struct mfi_softc *sc)			\
	{								\
		TAILQ_INIT(&sc->mfi_ ## name);				\
		MFIQ_INIT(sc, index);					\
	}								\
	static __inline void						\
	mfi_enqueue_ ## name (struct mfi_command *cm)			\
	{								\
		if ((cm->cm_flags & MFI_ON_MFIQ_MASK) != 0) {		\
			printf("command %p is on another queue, "	\
			    "flags = %#x\n", cm, cm->cm_flags);		\
			panic("command is on another queue");		\
		}							\
		TAILQ_INSERT_TAIL(&cm->cm_sc->mfi_ ## name, cm, cm_link); \
		cm->cm_flags |= MFI_ON_ ## index;			\
		MFIQ_ADD(cm->cm_sc, index);				\
	}								\
	static __inline void						\
	mfi_requeue_ ## name (struct mfi_command *cm)			\
	{								\
		if ((cm->cm_flags & MFI_ON_MFIQ_MASK) != 0) {		\
			printf("command %p is on another queue, "	\
			    "flags = %#x\n", cm, cm->cm_flags);		\
			panic("command is on another queue");		\
		}							\
		TAILQ_INSERT_HEAD(&cm->cm_sc->mfi_ ## name, cm, cm_link); \
		cm->cm_flags |= MFI_ON_ ## index;			\
		MFIQ_ADD(cm->cm_sc, index);				\
	}								\
	static __inline struct mfi_command *				\
	mfi_dequeue_ ## name (struct mfi_softc *sc)			\
	{								\
		struct mfi_command *cm;					\
									\
		if ((cm = TAILQ_FIRST(&sc->mfi_ ## name)) != NULL) {	\
			if ((cm->cm_flags & MFI_ON_ ## index) == 0) {	\
				printf("command %p not in queue, "	\
				    "flags = %#x, bit = %#x\n", cm,	\
				    cm->cm_flags, MFI_ON_ ## index);	\
				panic("command not in queue");		\
			}						\
			TAILQ_REMOVE(&sc->mfi_ ## name, cm, cm_link);	\
			cm->cm_flags &= ~MFI_ON_ ## index;		\
			MFIQ_REMOVE(sc, index);				\
		}							\
		return (cm);						\
	}								\
	static __inline void						\
	mfi_remove_ ## name (struct mfi_command *cm)			\
	{								\
		if ((cm->cm_flags & MFI_ON_ ## index) == 0) {		\
			printf("command %p not in queue, flags = %#x, " \
			    "bit = %#x\n", cm, cm->cm_flags,		\
			    MFI_ON_ ## index);				\
			panic("command not in queue");			\
		}							\
		TAILQ_REMOVE(&cm->cm_sc->mfi_ ## name, cm, cm_link);	\
		cm->cm_flags &= ~MFI_ON_ ## index;			\
		MFIQ_REMOVE(cm->cm_sc, index);				\
	}								\
struct hack

MFIQ_COMMAND_QUEUE(free, MFIQ_FREE);
MFIQ_COMMAND_QUEUE(ready, MFIQ_READY);
MFIQ_COMMAND_QUEUE(busy, MFIQ_BUSY);

static __inline void
mfi_initq_bio(struct mfi_softc *sc)
{
	bioq_init(&sc->mfi_bioq);
	MFIQ_INIT(sc, MFIQ_BIO);
}

static __inline void
mfi_enqueue_bio(struct mfi_softc *sc, struct bio *bp)
{
	bioq_insert_tail(&sc->mfi_bioq, bp);
	MFIQ_ADD(sc, MFIQ_BIO);
}

static __inline struct bio *
mfi_dequeue_bio(struct mfi_softc *sc)
{
	struct bio *bp;

	if ((bp = bioq_first(&sc->mfi_bioq)) != NULL) {
		bioq_remove(&sc->mfi_bioq, bp);
		MFIQ_REMOVE(sc, MFIQ_BIO);
	}
	return (bp);
}

static __inline void
mfi_print_sense(struct mfi_softc *sc, void *sense)
{
	int error, key, asc, ascq;

	scsi_extract_sense((struct scsi_sense_data *)sense,
	    &error, &key, &asc, &ascq);
	device_printf(sc->mfi_dev, "sense error %d, sense_key %d, "
	    "asc %d, ascq %d\n", error, key, asc, ascq);
}


#define MFI_WRITE4(sc, reg, val)	bus_space_write_4((sc)->mfi_btag, \
	sc->mfi_bhandle, (reg), (val))
#define MFI_READ4(sc, reg)		bus_space_read_4((sc)->mfi_btag, \
	(sc)->mfi_bhandle, (reg))
#define MFI_WRITE2(sc, reg, val)	bus_space_write_2((sc)->mfi_btag, \
	sc->mfi_bhandle, (reg), (val))
#define MFI_READ2(sc, reg)		bus_space_read_2((sc)->mfi_btag, \
	(sc)->mfi_bhandle, (reg))
#define MFI_WRITE1(sc, reg, val)	bus_space_write_1((sc)->mfi_btag, \
	sc->mfi_bhandle, (reg), (val))
#define MFI_READ1(sc, reg)		bus_space_read_1((sc)->mfi_btag, \
	(sc)->mfi_bhandle, (reg))

MALLOC_DECLARE(M_MFIBUF);

#define MFI_RESET_WAIT_TIME 180
#define MFI_CMD_TIMEOUT 360
#define MFI_SYS_PD_IO	0
#define MFI_LD_IO	1
#define SKINNY_MEMORY 0x02000000
#ifdef MFI_DEBUG
extern void mfi_print_cmd(struct mfi_command *cm);
extern void mfi_dump_cmds(struct mfi_softc *sc);
extern void mfi_validate_sg(struct mfi_softc *, struct mfi_command *, const char *, int );
#define MFI_PRINT_CMD(cm)	mfi_print_cmd(cm)
#define MFI_DUMP_CMDS(sc)	mfi_dump_cmds(sc)
#define MFI_VALIDATE_CMD(sc, cm) mfi_validate_sg(sc, cm, __FUNCTION__, __LINE__)
#else
#define MFI_PRINT_CMD(cm)
#define MFI_DUMP_CMDS(sc)
#define MFI_VALIDATE_CMD(sc, cm)
#endif

extern void mfi_release_command(struct mfi_command *cm);

#endif /* _MFIVAR_H */
