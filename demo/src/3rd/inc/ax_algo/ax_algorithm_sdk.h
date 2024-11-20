#ifndef __AX_ALGORITHM_SDK_H__
#define __AX_ALGORITHM_SDK_H__

#ifdef __cplusplus
extern "C"
{
#endif
#define AX_ALGORITHM_MAX_OBJ_NUM 32
    typedef enum _color_space_e
    {
        ax_color_space_unknown,
        ax_color_space_nv12,
        ax_color_space_nv21,
        ax_color_space_bgr,
        ax_color_space_rgb,
    } ax_color_space_e;

    typedef struct _bbox_t
    {
        float x, y, w, h;
    } ax_bbox_t;

    typedef struct _image_t
    {
        unsigned long long int pPhy;
        void *pVir;
        unsigned int nSize;
        unsigned int nWidth;
        unsigned int nHeight;
        ax_color_space_e eDtype;
        int tStride_W;

    } ax_image_t;

    typedef struct _result_t
    {
        struct
        {
            ax_bbox_t bbox;
            float score;
            int label;
            int status;
            unsigned long int track_id;
        } objects[AX_ALGORITHM_MAX_OBJ_NUM];
        int n_objects;
    } ax_result_t;

    typedef void *ax_algorithm_handle_t;

    typedef struct _algorithm_init_t
    {
        char model_file[256];
    } ax_algorithm_init_t;

    int ax_algorithm_init(ax_algorithm_init_t *init_info, ax_algorithm_handle_t *handle);
    void ax_algorithm_deinit(ax_algorithm_handle_t handle);
    int ax_algorithm_inference(ax_algorithm_handle_t handle, ax_image_t *image, ax_result_t *result);

    int ax_create_image(int width, int height, int stride, ax_color_space_e color, ax_image_t *image);
    void ax_release_image(ax_image_t *image);

#ifdef __cplusplus
}

#endif

#endif // __AX_ALGORITHM_SDK_H__