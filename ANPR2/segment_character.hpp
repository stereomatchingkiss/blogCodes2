#ifndef SEGMENT_CHARACTER_HPP
#define SEGMENT_CHARACTER_HPP

#include <opencv2/core.hpp>

#include <vector>

class segment_character
{
public:
    using contour_type = std::vector<cv::Point>;
    using contours_type = std::vector<contour_type>;

    segment_character() = default;

    /**
     * @param min_char_width minimum width of the characters
     * @param min_char_num minimum number of the license plate characters
     * @param max_char_num maxinum number of the license plate characters
     */
    segment_character(size_t min_char_width,
                      size_t min_char_num,
                      size_t max_char_num);

    /**
     * detect the characters of license plate
     * @param input input image
     * @param contours contours of candidate plates
     * @return true if able to detect characters and vice versa
     */
    bool detect_characters(cv::Mat const &input,                           
                           contour_type const &contours);

    /**
     * Get the bird eyes view of the license plate
     * @return bird eyes view of the license plate
     */
    cv::Mat const& get_bird_eyes_plate() const;
    /**
     * Get the contours of characters
     * @return contours of characters
     * @warning the contours is associated to the
     * bird eyes plate
     */
    contours_type const& get_chars_contours() const;

    contours_type& get_chars_contours();

    void set_img_name(std::string const &value);
    void set_min_char_width(size_t value);
    void set_min_char_num(size_t value);    
    void set_show_debug_message(bool value);

private:
    void binarize_plate();
    void bird_eyes_view_debug_message(cv::Mat const &input,
                                      contour_type const &contour,
                                      cv::Point2f const (&points)[4]);
    void generate_bird_eyes_view(cv::Mat const &input, contour_type const &contour);
    void generate_components();

    bool is_character_candidate(contour_type const &contour) const;

    void show_chars_component(int j, size_t i, contours_type contours);
    void show_chars_contour();
    void split_character();

    cv::Mat bird_eyes_plate_;
    contours_type chars_contour_;
    std::vector<cv::Mat> chars_mask_;
    bool debug_ = false;    
    std::vector<cv::Mat> hsv_split_;
    cv::Mat hsv_;
    std::string img_name_;
    cv::Mat intensity_; //v channel of hsv
    cv::Mat threshold_;

    size_t min_char_width_ = 10;
    size_t min_char_num_ = 6;
    size_t max_char_num_ = 15;
};

#endif
